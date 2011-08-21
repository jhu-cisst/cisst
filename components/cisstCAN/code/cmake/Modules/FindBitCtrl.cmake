
set( BitCtrlFound FALSE )

set( BITCTRL_SEARCH_PATH /opt /usr /usr/local )

# search for the can.h file

find_path( BITCTRL_DIR include/can.h ${BITCTRL_SEARCH_PATH} )

if( BITCTRL_DIR ) 

  set( BitCtrlFound TRUE )
  set( BITCTRL_INCLUDE_DIR ${BITCTRL_DIR} ${BITCTRL_DIR}/include )

endif( BITCTRL_DIR )

mark_as_advanced( BITCTRL_DIR )




