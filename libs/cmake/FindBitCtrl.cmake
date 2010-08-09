
set( BITCTRL_SEARCH_PATH /opt /usr /usr/local )

find_path( BITCTRL_DIR include/can.h ${BITCTRL_SEARCH_PATH} )

if( BITCTRL_DIR ) 

  set( BITCTRL_INCLUDE_DIR ${BITCTRL_DIR}/include )
  set( BITCTRL_FOUND true )

endif( BITCTRL_DIR )


