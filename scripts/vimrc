"====================================================================
" vim settings by Vijay Anand.R
"
" http://dotfiles.org/search?q=ctermfg  <<Reference>>
" http://vim.runpaint.org/vim-recipes.pdf
" http://tnerual.eriogerg.free.fr/vimqrc.html <<Reference>>
" Vim with tags
" http://www.linuxjournal.com/article/8289
"
"=====================================================================

set nocompatible              " be iMproved, required
filetype off                  " required

" set the runtime path to include Vundle and initialize
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()
"Plugin 'hari-rangarajan/CCTree'
Plugin 'farmergreg/vim-lastplace'
Plugin 'junegunn/fzf'
Plugin 'c.vim'
Plugin 'Townk/vim-autoclose'
"Plugin 'Yggdroot/indentLine'
Plugin 'VundleVim/Vundle.vim'
Plugin 'nathanalderson/yang.vim'
Plugin 'twerth/ir_black'
Plugin 'tomasr/molokai'
Plugin 'scrooloose/syntastic'
Plugin 'dense-analysis/ale'
Plugin 'jistr/vim-nerdtree-tabs'
Plugin 'kien/ctrlp.vim'
"Plugin 'airblade/vim-gitgutter'
Plugin 'easymotion/vim-easymotion'
Plugin 'tpope/vim-fugitive'
Plugin 'tpope/vim-rhubarb'
"Plugin 'vim-airline/vim-airline'
Plugin 'bogado/file-line'
Plugin 'jpalardy/spacehi.vim'
"<TAB> for auto completion while searching
Plugin 'vim-scripts/SearchComplete'
"Plugin 'vim-scripts/highlight.vim'
"Plugin 'MattesGroeger/vim-bookmarks'
Plugin 'kshenoy/vim-signature'
Plugin 'majutsushi/tagbar'
"Plugin 'vim-scripts/taglist.vim'
":FixWhiteSpace removes all white space
Plugin 'bronson/vim-trailing-whitespace'
"Use F5
Plugin 'sjl/gundo.vim'
Plugin 'vim-scripts/a.vim'
Plugin 'Raimondi/delimitMate'
":MRU to list MRU files.
Plugin 'vim-scripts/mru.vim'
":yanks
Plugin 'maxbrunsfeld/vim-yankstack'
Plugin 'scrooloose/nerdtree'
Plugin 'mhinz/vim-startify'
Plugin 'dracula/vim', { 'name': 'dracula' }
Plugin 'asenac/vim-opengrok'
"Plugin 'myusuf3/numbers.vim'
"Plugin 'powerline/powerline'
"Plugin 'Yggdroot/indentLine'
"Plugin 'altercation/vim-colors-solarized'
"au FileType python Plugin 'vim-scripts/python.vim'
"au BufNewFile,BufRead *.py Plugin 'vim-scripts/python.vim'
"Plugin 'vim-scripts/python.vim'
"Plugin 'christoomey/vim-tmux-navigator'
"Plugin 'honza/vim-snippets'
"Plugin 'kien/rainbow_parentheses.vim'
"Plugin 'luochen1990/rainbow'

call vundle#end()            " required
syntax on
filetype plugin indent on    " required

let g:gundo_right=1

set shiftwidth=4
set softtabstop=2
set tabstop=2
set expandtab
set smarttab
set cindent
set autoindent
set smartindent
set guifont=7x14
set ruler
set et
set modeline                   " Enable modeline.
set esckeys                    " Cursor keys in insert mode.
"set mouse=a                  " mouse support in all modes
set mousehide                  " hide the mouse when typing text
"set columns=80
set textwidth=80
set more
set scrolloff=5                " keep at least 5 lines above/below cursor
set sidescrolloff=5            " keep at least 5 columns left/right of cursor
set history=10                 " remember the last 200 commands
set autowrite                  " Automatically save before commands like :next
                               " and :make

"set cursorline
set t_Co=256
"set autochdir                  " always switch to the current file directory
"set backspace=indent,eol,start "make backspace a more flexible
" window spacing
set lazyredraw                  " don't redraw when running macros
"set number                      " show line number on each line

" this makes the mouse paste a block of text without formatting it
" (good for code)
map <MouseMiddle> <esc>"*p

syntax on
"syntax enable "Enable syntax hl
"syntax enable "Enable syntax hl

" ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
" searching...
set hlsearch                   " enable search highlight globally.
set incsearch                  " show matches as soon as possible.
set smartcase
set showmatch                  " show matching brackets when typing.
set ignorecase                 " case insensitive search.
set hidden
" Better command-line completion
set wildmenu

" ========================================================================================
" Make Vim to handle long lines nicely.
set wrap
set linebreak
set textwidth=0
set wrapmargin=0
set formatoptions-=t
set formatoptions+=l
"set colorcolumn=+1
"set formatoptions=qrn1
"set colorcolumn=79


"==========================================================================="
" Working with split screen nicely
" Resize Split When the window is resized"
au VimResized * :wincmd =


" ======================================================================================== asdfsdf sdfasdf
" To  show special characters in Vim
"set list
" Show partial commands in the last line of the screen
set showcmd
" set incsearch                  " type-ahead-find.
" Press Space to turn off highlighting and clear any message already displayed.
noremap <silent> <Space> :silent noh<Bar>echo<CR>
"map _F [[k"xy$``:echo @x<CR>
"map _F mk][%?^[A-Za-z_].*(<CR>V"ky`k:echo "<C-R>k"<CR>
" Press F4 to toggle highlighting on/off.
" noremap <F4> :set hls!<CR>
" ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

"au BufEnter,BufNew *.reg set filetype=
"au BufEnter,BufNew *.reg runtime! syntax/c.vim
"au BufEnter,BufNew *.reg syn keyword Macro END DEFINE DATA LIST LOOP STUB CASE RETVAL VALUE CASE REGISTRY
"au BufEnter,BufNew *.reg syn region String matchgroup=Macro start="^BEGIN\s\+REGISTRY\s\+" end="$" keepend
"au BufEnter,BufNew *.reg syn region Comment start="#" skip="\\$" end="$" keepend
"au BufEnter *.reg[ch] set filetype=c


"=======================================================================
"FOLD options
"======================================================================
"set foldenable
"set foldmarker={,}
"set foldmethod=marker
"set foldmethod=indent
"zf#j  creates a fold from the cursor down  #  lines.
"zf/string creates a fold from the cursor to string .
"zj moves the cursor to the next fold.
"zk moves the cursor to the previous fold.
"zo opens a fold at the cursor.
"zO opens all folds at the cursor.
"zm increases the foldlevel by one.
"zM closes all open folds.
"zr decreases the foldlevel by one.
"zR decreases the foldlevel to zero -- all folds will be open.
"zd deletes the fold at the cursor.
"zE deletes all folds.
"[z move to start of open fold.
"]z move to end of open fold.
"======================================================================

:command WQ wq
:command Wq wq
:command W w
:command Q q


"======================================================================
"status line
"======================================================================
" Show status line always
set laststatus=2
set statusline=FILE=>\%t%h%m%r%=[R:%l,C:%c%V]\ POS->\%P
set showcmd "show command in the status line
set cmdheight=1
set showmode "show mode in below

"===========================================================================
" CTAGS
"===========================================================================
" If you want to go to the definition of anything,
" be it a function, macro or anything else, simply press Ctrl-]
" when the cursor is positioned on it. Thus, from invocation,
" we can move to the definition. It takes you there no matter which
" file contains it.If you want to go back to what you were reading,
" press Ctrl-T, and you return to where you left.
" You can jump to another invocation from there by pressing Ctrl-] again.
" You can continue this process ad infinitum, and
" you can keep coming back by pressing Ctrl-T.
"
" Another way to find a function definition if you know only a part of the name is:

" :ta /function

" This command takes you to the first match if there are multiple matches.
" You can go to the next match with :tn.

" If there are multiple definitions and you want to choose among them,
" you can press G Ctrl-] or type :tselect <tagname>.
" This way you can modify the source code by navigating with tags
" without even knowing which file contains what.

set tags=.tags,./.tags,tags
set title


"12.6.2 Running Cscope Queries from Vim
"======================================
"
"To run any of the cscope queries, use:
"
":cs find {querytype} name
"
"The supported query types {querytype} are:
"
"0 or s: Find this C symbol
"
"1 or g: Find this definition
"
"2 or d: Find functions called by this function
"
"3 or c: Find functions calling this function
"
"4 or t: Find this text string
"
"6 or e: Find this egrep pattern
"
"7 or f: Find this file
"
"8 or i: Find files #including this file
"
"For example, to find the definition of the process_create() function, you can
"use:
"
":cs find g process_create
"
"The above command displays the Cscope query results with a number assigned for
"each line in the output and you can select an entry from the result by the
"corresponding number and jump to it.
"12.6.3 Using quickfix List for Browsing Query Results
"
"You can configure Vim to add the cscope query results to a quickfix list and use
"the quickfix commands to browse the query results. To enable this feature, set
"the cscopequickfix option:
"
"set cscopequickfix=s-,c-,d-,i-,t-,e-,g-
"set cscopequickfix=s-,c-,d-,i-,t-,e-,g-

"
"The above setting configures Vim to use the quickfix list for the Cscope
"s,c,d,i,t,e and g queries. For every query a new quickfix list is created.
"
"You can browse the quickfix list and jump to the entries in the Cscope query
"result using the :cnext, :cprev, :cnfile, :cpfile, :cfirst, :crewind, :clast,
"and :clist commands. To jump to the current entry in the list use the :cc
"command.
"
"To go to the previous quickfix list, use the :colder command and to go the next
"quickfix list, use the :cnewer command. You can open the quickfix window using
"the :cwindow or :copen command.
"12.6.4 Using Cscope Database for Tags
"
"To perform a tag selection and jump using the cscope database, use the :cstag
"name command. To use the cscope database for the :tag and CTRL-] commands to
"jump to a tag, set the cscopetag option.
"
"
"===========================================================================


""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" CSCOPE settings for vim
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
"
" This file contains some boilerplate settings for vim's cscope interface,
" plus some keyboard mappings that I've found useful.
"
" USAGE:
" -- vim 6:     Stick this file in your ~/.vim/plugin directory (or in a
"               'plugin' directory in some other directory that is in your
"               'runtimepath'.
"
" -- vim 5:     Stick this file somewhere and 'source cscope.vim' it from
"               your ~/.vimrc file (or cut and paste it into your .vimrc).
"
" NOTE:
" These key maps use multiple keystrokes (2 or 3 keys).  If you find that vim
" keeps timing you out before you can complete them, try changing your timeout
" settings, as explained below.
"
" Happy cscoping,
"
" Jason Duell       jduell@alumni.princeton.edu     2002/3/7
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""


" This tests to see if vim was configured with the '--enable-cscope' option
" when it was compiled.  If it wasn't, time to recompile vim...
if has("cscope")

    """"""""""""" Standard cscope/vim boilerplate

    " use both cscope and ctag for 'ctrl-]', ':ta', and 'vim -t'
    set cscopetag

    " check cscope for definition of a symbol before checking ctags: set to 1
    " if you want the reverse search order.
    set csto=0
    set cst
    set csto=1


    " add any cscope database in current directory
    if filereadable("cscope.out")
        cs add cscope.out
    " else add the database pointed to by environment variable
    elseif $CSCOPE_DB != ""
        cs add $CSCOPE_DB
    endif

    " show msg when any other cscope db added
    set cscopeverbose


    """"""""""""" My cscope/vim key mappings
    "
    " The following maps all invoke one of the following cscope search types:
    "
    "   's'   symbol: find all references to the token under cursor
    "   'g'   global: find global definition(s) of the token under cursor
    "   'c'   calls:  find all calls to the function name under cursor
    "   't'   text:   find all instances of the text under cursor
    "   'e'   egrep:  egrep search for the word under cursor
    "   'f'   file:   open the filename under cursor
    "   'i'   includes: find files that include the filename under cursor
    "   'd'   called: find functions that function under cursor calls
    "
    " Below are three sets of the maps: one set that just jumps to your
    " search result, one that splits the existing vim window horizontally and
    " diplays your search result in the new window, and one that does the same
    " thing, but does a vertical split instead (vim 6 only).
    "
    " I've used CTRL-\ and CTRL-@ as the starting keys for these maps, as it's
    " unlikely that you need their default mappings (CTRL-\'s default use is
    " as part of CTRL-\ CTRL-N typemap, which basically just does the same
    " thing as hitting 'escape': CTRL-@ doesn't seem to have any default use).
    " If you don't like using 'CTRL-@' or CTRL-\, , you can change some or all
    " of these maps to use other keys.  One likely candidate is 'CTRL-_'
    " (which also maps to CTRL-/, which is easier to type).  By default it is
    " used to switch between Hebrew and English keyboard mode.
    "
    " All of the maps involving the <cfile> macro use '^<cfile>$': this is so
    " that searches over '#include <time.h>" return only references to
    " 'time.h', and not 'sys/time.h', etc. (by default cscope will return all
    " files that contain 'time.h' as part of their name).


    " To do the first type of search, hit 'CTRL-\', followed by one of the
    " cscope search types above (s,g,c,t,e,f,i,d).  The result of your cscope
    " search will be displayed in the current window.  You can use CTRL-T to
    " go back to where you were before the search.
    "

    nmap <C-\>s :cs find s <C-R>=expand("<cword>")<CR><CR>
    nmap <C-\>g :cs find g <C-R>=expand("<cword>")<CR><CR>
    nmap <C-\>c :cs find c <C-R>=expand("<cword>")<CR><CR>
    nmap <C-\>t :cs find t <C-R>=expand("<cword>")<CR><CR>
    nmap <C-\>e :cs find e <C-R>=expand("<cword>")<CR><CR>
    nmap <C-\>f :cs find f <C-R>=expand("<cfile>")<CR><CR>
    nmap <C-\>i :cs find i ^<C-R>=expand("<cfile>")<CR>$<CR>
    nmap <C-\>d :cs find d <C-R>=expand("<cword>")<CR><CR>
"You can browse the quickfix list and jump to the entries in the Cscope query
"result using the :cnext, :cprev, :cnfile, :cpfile, :cfirst, :crewind, :clast,
"and :clist commands. To jump to the current entry in the list use the :cc
"command.


    nmap <C-\>n :cnext <CR>
    nmap <C-\>p :cprev <CR>
    nmap <C-\>f :cfirst <CR>
    nmap <C-\>l :clast <CR>
    nmap <C-\>r :crewind <CR>
    nmap <C-\>cl :clist <CR>
    nmap <C-\>cc :cc <CR>
    nmap <C-\>cn :cnfile <CR>
    nmap <C-\>cp :cpfile <CR>
    nmap <C-\>cp :cpfile <CR>

"
"To go to the previous quickfix list, use the :colder command and to go the next
"quickfix list, use the :cnewer command. You can open the quickfix window using
"the :cwindow or :copen command.


    " Using 'CTRL-spacebar' (intepreted as CTRL-@ by vim) then a search type
    " makes the vim window split horizontally, with search result displayed in
    " the new window.
    "
    " (Note: earlier versions of vim may not have the :scs command, but it
    " can be simulated roughly via:
    "    nmap <C-@>s <C-W><C-S> :cs find s <C-R>=expand("<cword>")<CR><CR>

    nmap <C-@>s :scs find s <C-R>=expand("<cword>")<CR><CR>
    nmap <C-@>g :scs find g <C-R>=expand("<cword>")<CR><CR>
    nmap <C-@>c :scs find c <C-R>=expand("<cword>")<CR><CR>
    nmap <C-@>t :scs find t <C-R>=expand("<cword>")<CR><CR>
    nmap <C-@>e :scs find e <C-R>=expand("<cword>")<CR><CR>
    nmap <C-@>f :scs find f <C-R>=expand("<cfile>")<CR><CR>
    nmap <C-@>i :scs find i ^<C-R>=expand("<cfile>")<CR>$<CR>
    nmap <C-@>d :scs find d <C-R>=expand("<cword>")<CR><CR>


    " Hitting CTRL-space *twice* before the search type does a vertical
    " split instead of a horizontal one (vim 6 and up only)
    "
    " (Note: you may wish to put a 'set splitright' in your .vimrc
    " if you prefer the new window on the right instead of the left

    nmap <C-@><C-@>s :vert scs find s <C-R>=expand("<cword>")<CR><CR>
    nmap <C-@><C-@>g :vert scs find g <C-R>=expand("<cword>")<CR><CR>
    nmap <C-@><C-@>c :vert scs find c <C-R>=expand("<cword>")<CR><CR>
    nmap <C-@><C-@>t :vert scs find t <C-R>=expand("<cword>")<CR><CR>
    nmap <C-@><C-@>e :vert scs find e <C-R>=expand("<cword>")<CR><CR>
    nmap <C-@><C-@>f :vert scs find f <C-R>=expand("<cfile>")<CR><CR>
    nmap <C-@><C-@>i :vert scs find i ^<C-R>=expand("<cfile>")<CR>$<CR>
    nmap <C-@><C-@>d :vert scs find d <C-R>=expand("<cword>")<CR><CR>



    """"""""""""" key map timeouts
    "
    " By default Vim will only wait 1 second for each keystroke in a mapping.
    " You may find that too short with the above typemaps.  If so, you should
    " either turn off mapping timeouts via 'notimeout'.
    "
    "set notimeout
    "
    " Or, you can keep timeouts, by uncommenting the timeoutlen line below,
    " with your own personal favorite value (in milliseconds):
    "
    "set timeoutlen=4000
    "
    " Either way, since mapping timeout settings by default also set the
    " timeouts for multicharacter 'keys codes' (like <F1>), you should also
    " set ttimeout and ttimeoutlen: otherwise, you will experience strange
    " delays as vim waits for a keystroke after you hit ESC (it will be
    " waiting to see if the ESC is actually part of a key code like <F1>).
    "
    "set ttimeout
    "
    " personally, I find a tenth of a second to work well for key code
    " timeouts. If you experience problems and have a slow terminal or network
    " connection, set it higher.  If you don't set ttimeoutlen, the value for
    " timeoutlent (default: 1000 = 1 second, which is sluggish) is used.
    "
    "set ttimeoutlen=100
"     if exists("cscope.out")
"         cs add cscope.out
"     endif
"     set csprg=/router/bin/cscope
"
endif

":source /users/vanandr/.vim_cscope


"------------------------------------------------------------------------------
" Correct typos.
"------------------------------------------------------------------------------

"iab beacuse    because
"iab becuase    because
"iab acn        can
"iab cna        can
"iab centre     center
"iab chnage     change
"iab chnages    changes
"iab chnaged    changed
"iab chnagelog  changelog
"iab Chnage     Change
"iab Chnages    Changes
"iab ChnageLog  ChangeLog
"iab debain     debian
"iab Debain     Debian
"iab defualt    default
"iab Defualt    Default
"iab differnt   different
"iab diffrent   different
"iab emial      email
"iab Emial      Email
"iab figth      fight
"iab figther    fighter
"iab fro        for
"iab fucntion   function
"iab ahve       have
"iab homepgae   homepage
"iab logifle    logfile
"iab lokk       look
"iab lokking    looking
"iab mial       mail
"iab Mial       Mail
"iab miantainer maintainer
"iab amke       make
"iab mroe       more
"iab nwe        new
"iab recieve    receive
"iab recieved   received
"iab erturn     return
"iab retrun     return
"iab retunr     return
"iab seperate   separate
"iab shoudl     should
"iab soem       some
"iab taht       that
"iab thta       that
"iab teh        the
"iab tehy       they
"iab truely     truly
"iab waht       what
"iab wiht       with
"iab whic       which
"iab whihc      which
"iab yuo        you
"iab databse    database
"iab versnio    version
"iab obnsolete  obsolete
"iab flase      false
"iab recrusive  recursive
"iab Recrusive  Recursive
"
"========================================================================

"========================================================================
"
"MAKE OPTION IN VIM
"
"========================================================================

"set list      Replaces all tabs in a file with ^I,
"              puts a $ at the end of each line.

"make args     To run make from within vim.
"              After running make if there are any errors,
"              vim takes you to the first error automatically.
"
"cnext         Takes you to the next make error.
"
"cprevious|cNext     Go to previous error.
"
"clast  Go to last error.
"
"crewind        Go to first error.
"
"cnfile         Go to first error on next file.
"
"cc     Display the current error.
"
"clist  Display a list of errors.
"
"clist x,y      List errors x through y.
"
"clist ,x       List errors 1 to x.
"
"clist x,       List errors x to the end.
"
"clist!         Vim suppresses all informational messages,
"                to see all messages use this command.
"
"cfile error-file       If you have already run vim and have a error file,
"                        you can tell vim about it. If error-file is not
"                        specified, the file specified by errorfile option
"                        is used.
"
"set errorfile=erris    Set a default error file.
"============================================================================

"===========================================================================
"colour schemes
"colorscheme slate
"===========================================================================
"just for the sake of highlight
"map <F6> :highlight Comment ctermfg=red <CR>
"map <F7> : highlight Comment ctermfg=blue cterm=bold <CR>

"if bufwinnr(1)
"  map + <C-W>+
"  map - <C-W>-
"endif
"
"highlight ErrorMsg     cterm=underline  cterm=bold   ctermfg=2 ctermbg=black
"highlight Comment      ctermfg=darkblue cterm=bold
"highlight Special      ctermfg=DarkMagenta guifg=SlateBlue
"highlight String       ctermfg=darkcyan
"
"highlight DiffAdd      ctermfg=NONE     ctermbg=22   cterm=NONE
"highlight DiffDelete   ctermfg=NONE     ctermbg=52   cterm=NONE
"highlight DiffChange   ctermfg=NONE     ctermbg=17   cterm=NONE
"highlight DiffText     ctermfg=NONE     ctermbg=NONE cterm=underline
"
"highlight VertSplit    ctermfg=16       ctermbg=23   cterm=NONE
"
"highlight Split        ctermfg=16       ctermbg=23   cterm=NONE
"highlight Statement ctermfg=yellow
"highlight Constant ctermfg=red

"set comments=sl:/*,mb:\ *,elx:\ */

"if has('cscope')
"  set cscopetag cscopeverbose
"
"  if has('quickfix')
"    set cscopequickfix=s-,c-,d-,i-,t-,e-
"  endif
"
"  cnoreabbrev csa cs add
"  cnoreabbrev csf cs find
"  cnoreabbrev csk cs kill
"  cnoreabbrev csr cs reset
"  cnoreabbrev css cs show
"  cnoreabbrev csh cs help
"
"  command -nargs=0 Cscope cs add $VIMSRC/src/cscope.out $VIMSRC/src
"endif
"colorscheme desert

" Set nice colors
" background for normal text is light grey
" Text below the last line is darker grey
" Cursor is green
" Constants are not underlined but have a slightly lighter background
"colorscheme ir_black
colorscheme molokai
highlight VertSplit    ctermfg=16       ctermbg=23   cterm=NONE
highlight Split        ctermfg=16       ctermbg=23   cterm=NONE
highlight Normal guibg=grey90
highlight Cursor guibg=Green guifg=NONE
highlight NonText guibg=grey80
highlight Constant gui=NONE guibg=grey95
highlight Search ctermfg=red ctermbg=none cterm=bold,underline
highlight Visual cterm=bold,underline ctermbg=59
highlight Comment ctermfg=6



"highlight ModeMsg      cterm=bold       ctermfg=2     ctermbg=black
"" set mode message ( --INSERT-- ) to green
"highlight StatusLine   ctermfg=darkred ctermbg=black cterm=bold
"highlight StatusLine   ctermfg=yellow  ctermbg=blue cterm=bold
"highlight StatusLineNC ctermfg=94       ctermbg=234   cterm=NONE

"set swapfile
"set dir=~/swapfiles

" Vim tabs
nnoremap tc :tabnew<Space>
nnoremap tn :tabnext<CR>
nnoremap tp :tabprev<CR>
nnoremap tf :tabfirst<CR>
nnoremap tl :tablast<CR>

nnoremap <c-l> :MRU<CR>
""noremap <c-g> :GundoToggle<CR>
nnoremap <c-s> :NERDTreeTabsToggle<CR>
nnoremap <c-o> :TagbarToggle<CR>

" GOTO know this.
" Use Ctrl+P and Ctrl+N to autofill the variables names etc.
"highlight BookmarkSign ctermbg=NONE ctermfg=160
highlight BookmarkLine ctermbg=red ctermfg=NONE
"let g:bookmark_highlight_lines = 1
set expandtab
set shiftwidth=2
set softtabstop=2
filetype plugin indent on
set tags=tags
set smartcase
filetype plugin indent on
syntax on
let anyfold_activate=1
set foldlevel=0

map <C-t><up> :tabr<cr>
map <C-t><down> :tabl<cr>
map <C-t><left> :tabp<cr>
map <C-t><right> :tabn<cr>
