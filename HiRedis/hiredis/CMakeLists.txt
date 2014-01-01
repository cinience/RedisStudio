



set(LIB_NAME hiredis)


set(LIB_SRCS 
hiredis.c  
net.c
sds.c
dict.c
async.c
)

#aux_source_directory(. LIB_SRCS)
 
#set(CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG} "-std=c99 -pedantic") 
#set(CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE} "-std=c99 -pedantic")
add_library(${LIB_NAME} STATIC ${LIB_SRCS})

