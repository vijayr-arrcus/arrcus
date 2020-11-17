/*	Sysfs attributes of bond slaves
 *
 *      Copyright (c) 2014 Scott Feldman <sfeldma@cumulusnetworks.com>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <linux/capability.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>

#include <net/bonding.h>

struct slave_attribute {
	struct attribute attr;
	ssize_t (*show)(struct slave *, char *);
  ssize_t (*store)(struct slave *, const char *, size_t);
};

#define SLAVE_ATTR(_name, _mode, _show, _store)				\
const struct slave_attribute slave_attr_##_name = {		\
	.attr = {.name = __stringify(_name),			\
		 .mode = _mode },				\
	.show	= _show,					\
  .store = _store,        \
};

#define SLAVE_ATTR_RO(_name)					\
	SLAVE_ATTR(_name, 0444, _name##_show, NULL)

#define SLAVE_ATTR_RW(_name)					\
	SLAVE_ATTR(_name, 0666, _name##_show, _name##_store)

static ssize_t fwd_state_show (struct slave *slave, char *buf)
{
  switch (bond_slave_fwd_state(slave)) {
    case BOND_FWD_STATE_ACTIVE:
      return sprintf(buf, "active\n");
    case BOND_FWD_STATE_INACTIVE:
      return sprintf(buf, "inactive\n");
    default:
      return sprintf(buf, "UNKNOWN\n");
  }
}

static ssize_t fwd_state_store (struct slave *slave, const char *buf, size_t len)
{
  int rc = 0;
  int fwd_state = BOND_FWD_STATE_ACTIVE;

  rc = kstrtoint(buf, 0, &fwd_state);

  if (fwd_state == BOND_FWD_STATE_ACTIVE ||
      fwd_state == BOND_FWD_STATE_INACTIVE) {
    slave->fwd_state = fwd_state;
  } else {
    rc = -EINVAL;
  }
  printk(KERN_ALERT "setting slave %s fwd state %d\n", slave->dev->name, fwd_state);
  return rc;
}
static SLAVE_ATTR_RW(fwd_state);

static ssize_t state_show(struct slave *slave, char *buf)
{
	switch (bond_slave_state(slave)) {
	case BOND_STATE_ACTIVE:
		return sprintf(buf, "active\n");
	case BOND_STATE_BACKUP:
		return sprintf(buf, "backup\n");
	default:
		return sprintf(buf, "UNKNOWN\n");
	}
}
static SLAVE_ATTR_RO(state);

static ssize_t mii_status_show(struct slave *slave, char *buf)
{
	return sprintf(buf, "%s\n", bond_slave_link_status(slave->link));
}
static SLAVE_ATTR_RO(mii_status);

static ssize_t link_failure_count_show(struct slave *slave, char *buf)
{
	return sprintf(buf, "%d\n", slave->link_failure_count);
}
static SLAVE_ATTR_RO(link_failure_count);

static ssize_t perm_hwaddr_show(struct slave *slave, char *buf)
{
	return sprintf(buf, "%*phC\n",
		       slave->dev->addr_len,
		       slave->perm_hwaddr);
}
static SLAVE_ATTR_RO(perm_hwaddr);

static ssize_t queue_id_show(struct slave *slave, char *buf)
{
	return sprintf(buf, "%d\n", slave->queue_id);
}
static SLAVE_ATTR_RO(queue_id);

static ssize_t ad_aggregator_id_show(struct slave *slave, char *buf)
{
	const struct aggregator *agg;

	if (BOND_MODE(slave->bond) == BOND_MODE_8023AD) {
		agg = SLAVE_AD_INFO(slave)->port.aggregator;
		if (agg)
			return sprintf(buf, "%d\n",
				       agg->aggregator_identifier);
	}

	return sprintf(buf, "N/A\n");
}
static SLAVE_ATTR_RO(ad_aggregator_id);

static ssize_t ad_actor_oper_port_state_show(struct slave *slave, char *buf)
{
	const struct port *ad_port;

	if (BOND_MODE(slave->bond) == BOND_MODE_8023AD) {
		ad_port = &SLAVE_AD_INFO(slave)->port;
		if (ad_port->aggregator)
			return sprintf(buf, "%u\n",
				       ad_port->actor_oper_port_state);
	}

	return sprintf(buf, "N/A\n");
}
static SLAVE_ATTR_RO(ad_actor_oper_port_state);

static ssize_t ad_partner_oper_port_state_show(struct slave *slave, char *buf)
{
	const struct port *ad_port;

	if (BOND_MODE(slave->bond) == BOND_MODE_8023AD) {
		ad_port = &SLAVE_AD_INFO(slave)->port;
		if (ad_port->aggregator)
			return sprintf(buf, "%u\n",
				       ad_port->partner_oper.port_state);
	}

	return sprintf(buf, "N/A\n");
}
static SLAVE_ATTR_RO(ad_partner_oper_port_state);

static const struct slave_attribute *slave_attrs[] = {
	&slave_attr_state,
	&slave_attr_mii_status,
	&slave_attr_link_failure_count,
	&slave_attr_perm_hwaddr,
	&slave_attr_queue_id,
	&slave_attr_ad_aggregator_id,
	&slave_attr_ad_actor_oper_port_state,
	&slave_attr_ad_partner_oper_port_state,
	NULL
};

#define to_slave_attr(_at) container_of(_at, struct slave_attribute, attr)
#define to_slave(obj)	container_of(obj, struct slave, kobj)

static ssize_t slave_show(struct kobject *kobj,
			  struct attribute *attr, char *buf)
{
	struct slave_attribute *slave_attr = to_slave_attr(attr);
	struct slave *slave = to_slave(kobj);

	return slave_attr->show(slave, buf);
}

static ssize_t slave_store(struct kobject *kobj, struct attribute *attr,
                            const char *buf, size_t len)
{
	struct slave_attribute *slave_attr = to_slave_attr(attr);
	struct slave *slave = to_slave(kobj);

  if (!slave_attr->store) {
    return -EIO;
  }

	return slave_attr->store(slave, buf, len);
}

static const struct sysfs_ops slave_sysfs_ops = {
	.show = slave_show,
  .store = slave_store,
};

static struct kobj_type slave_ktype = {
#ifdef CONFIG_SYSFS
	.sysfs_ops = &slave_sysfs_ops,
#endif
};

int bond_sysfs_slave_add(struct slave *slave)
{
	const struct slave_attribute **a;
	int err;

	err = kobject_init_and_add(&slave->kobj, &slave_ktype,
				   &(slave->dev->dev.kobj), "bonding_slave");
	if (err)
		return err;

	for (a = slave_attrs; *a; ++a) {
		err = sysfs_create_file(&slave->kobj, &((*a)->attr));
		if (err) {
			kobject_put(&slave->kobj);
			return err;
		}
	}

	return 0;
}

void bond_sysfs_slave_del(struct slave *slave)
{
	const struct slave_attribute **a;

	for (a = slave_attrs; *a; ++a)
		sysfs_remove_file(&slave->kobj, &((*a)->attr));

	kobject_put(&slave->kobj);
}
