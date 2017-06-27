add_executable(helloworld main.cpp)
target_link_libraries(helloworld groceries)
set_target_properties(helloworld PROPERTIES FOLDER GROCERIES/TESTS)