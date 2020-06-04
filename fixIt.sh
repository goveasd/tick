pushd install/bin
install_name_tool -add_rpath @executable_path/../lib tickserver
install_name_tool -add_rpath @executable_path/../lib tickclient
popd
