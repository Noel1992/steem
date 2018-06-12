add_executable( jsonrpc2_client_demo main.cpp )
if( UNIX AND NOT APPLE )
  set(rt_library rt )
endif()

if( STEEM_STATIC_BUILD )
   target_link_libraries( jsonrpc2_client_demo PRIVATE
                          "-static-libstdc++ -static-libgcc -lreadline"
                          graphene_net steem_protocol steem_utilities condenser_api_plugin fc ${CMAKE_DL_LIBS} ${PLATFORM_SPECIFIC_LIBS} )
else( STEEM_STATIC_BUILD )
   target_link_libraries( jsonrpc2_client_demo PRIVATE
                          graphene_net steem_protocol steem_utilities condenser_api_plugin fc ${CMAKE_DL_LIBS} ${PLATFORM_SPECIFIC_LIBS} )
endif( STEEM_STATIC_BUILD )

if( CLANG_TIDY_EXE )
   set_target_properties(
    jsonrpc2_client_demo PROPERTIES
      CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
   )
endif( CLANG_TIDY_EXE )

if(MSVC)
  set_source_files_properties( main.cpp PROPERTIES COMPILE_FLAGS "/bigobj" )
endif(MSVC)

install( TARGETS
   jsonrpc2_client_demo

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
