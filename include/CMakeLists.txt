file(GLOB LIB_OBJECTS_HEADERS
        "open62541cpp/objects/*.h"
)

install(FILES ${LIB_OBJECTS_HEADERS} DESTINATION include/open62541cpp/objects)

file(GLOB LIB_HEADERS
        "open62541cpp/*.h"
)

install(FILES ${LIB_HEADERS} DESTINATION include/open62541cpp)
