/* CS8776958 */
cint_reset();
typedef struct bcm_dnx_policer_cfg_t {
  int base_pointer;
  int base_engine;
  int total_meters;
  int start_engine;
  int db_id;
};

bcm_dnx_policer_cfg_t policer_cfg[2] = {
   { 0, 0, 2048, 0, 0 },
   { 0, 0, 2048, 0, 0 }
};

int policer_db_hdl[2];
bcm_policer_t policer_ids[2];

int
bcm_dnx_sdk_policer_init(int unit)
{
  bcm_policer_database_config_t db_cfg;
  bcm_policer_engine_t engine;
  bcm_policer_database_attach_config_t db_attach_cfg;
  bcm_error_t rv;
  int flags = 0, i, core;
  int db_hdl, nof_elements;
  unsigned engines;
  unsigned *engine_size = &engines;

  for(core = 0; core < 2; core++) {
    bcm_policer_engine_t_init(&engine);
    bcm_policer_database_attach_config_t_init(&db_attach_cfg);
    bcm_policer_database_config_t_init(&db_cfg);

    db_cfg.expansion_enable = FALSE;
    /* ingress special engine is single bucket */
    db_cfg.is_single_bucket = TRUE;
    BCM_POLICER_DATABASE_HANDLE_SET(db_hdl, TRUE, 0, core, policer_cfg[core].db_id);
    rv = bcm_policer_database_create(unit, flags, db_hdl, &db_cfg);
    if (rv != BCM_E_NONE) {
      printf( "Error in bcm_policer_database_create : %s\n", bcm_errmsg(rv));
      return -1;
    }

    db_attach_cfg.policer_database_handle = db_hdl;
    /* engine 0 is the single ingress special engine */
    engine.engine_id = 0;
    engine.core_id = core;
    rv = bcm_policer_engine_database_attach(unit, flags, &engine,
                                            &db_attach_cfg);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_policer_engine_database_attach (engine=%d, section=%d) : %s\n",
          engine.engine_id, engine.section, bcm_errmsg(rv));
      return -1;
    }

    rv = bcm_policer_database_enable_set(unit, flags, db_hdl, TRUE);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_policer_database_enable_set : %s\n", bcm_errmsg(rv));
      return -1;
    }
    policer_db_hdl[core] = db_hdl;
  }
  return 0;
}

int
bcm_dnx_sdk_policer_create_mode(int unit,
                           int flags, int cbits, int cburst,
                           bcm_policer_mode_t mode, int meter_idx)
{
  bcm_policer_config_t pol_cfg;
  bcm_policer_t policer_id;
  unsigned int core;

  bcm_error_t rv;

  for (core = 0; core < 2; core++) {
    bcm_policer_config_t_init(&pol_cfg);
    pol_cfg.flags |= BCM_POLICER_COLOR_BLIND | flags;
    pol_cfg.mode = mode;
    pol_cfg.ckbits_sec = cbits;
    pol_cfg.ckbits_burst = cburst;
    pol_cfg.core_id = core;

    BCM_POLICER_ID_SET(policer_id, policer_db_hdl[core], meter_idx);

    policer_ids[core] = policer_id;

    rv = bcm_policer_create(unit, &pol_cfg, &policer_id);
    if (rv != BCM_E_NONE) {
      printf("bcm_policer_create failed: %s\n", bcm_errmsg(rv));
      return -1;
    }
  }
  return 0;
}

void
bcm_dnx_sdk_policer_destroy(int unit, bcm_policer_t policer_id)
{
  bcm_error_t rv;
  rv = bcm_policer_destroy(unit, policer_id);
  if (rv != BCM_E_NONE) {
    printf("bcm_policer_destroy failed: %s\n", bcm_errmsg(rv));
  }
}

void
test(int unit)
{
  print bcm_dnx_sdk_policer_init(unit);
  print bcm_dnx_sdk_policer_create_mode(unit,
                             BCM_POLICER_MODE_PACKETS, 100, 1000,
                             bcmPolicerModeCommitted, 10);
  print bcm_dnx_sdk_policer_destroy(unit, policer_ids[0]);
  print bcm_dnx_sdk_policer_destroy(unit, policer_ids[1]);
}
