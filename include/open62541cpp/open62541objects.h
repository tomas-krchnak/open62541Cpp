/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#ifndef OPEN62541OBJECTS_H
#define OPEN62541OBJECTS_H

#if defined(_MSC_VER)
// get rid of template not exported warnings - warning is meaningless as it cannot be fixed
#pragma warning(disable : 4251)
#endif
//

// Backup value of UA_LOGLEVEL. IT is already defined in config.h
// so we unset it here to avoid redefined warning, then define it again afterwards.
#define UA_LOGLEVEL_ORIG UA_LOGLEVEL
#undef UA_LOGLEVEL
#ifdef UA_ENABLE_AMALGAMATION
#include "open62541.h"
#else
#include "open62541/config.h"
#include "open62541/statuscodes.h"
#include "open62541/nodeids.h"
#include "open62541/common.h"
#include "open62541/types.h"
#include "open62541/util.h"
#include "open62541/client.h"
#include "open62541/server.h"
#include "open62541/architecture_definitions.h"
#include "open62541/server_pubsub.h"
#include "open62541/types_generated.h"
#include "open62541/network_tcp.h"
#include "open62541/client_config_default.h"
#include "open62541/client_highlevel_async.h"
#include "open62541/types_generated_handling.h"
#include "open62541/architecture_functions.h"
//#include "open62541/posix/ua_architecture.h"
#include "open62541/server_config_default.h"
#include "open62541/client_subscriptions.h"
#include "open62541/client_highlevel.h"
#include "open62541/plugin/historydatabase.h"
#include "open62541/plugin/log_syslog.h"
#include "open62541/plugin/network.h"
#include "open62541/plugin/historydata/history_database_default.h"
#include "open62541/plugin/historydata/history_data_backend.h"
#include "open62541/plugin/historydata/history_data_gathering.h"
#include "open62541/plugin/historydata/history_data_gathering_default.h"
#include "open62541/plugin/historydata/history_data_backend_memory.h"
#include "open62541/plugin/log.h"
#include "open62541/plugin/nodestore.h"
#include "open62541/plugin/pki.h"
#include "open62541/plugin/pubsub.h"
#include "open62541/plugin/nodestore_default.h"
#include "open62541/plugin/log_stdout.h"
#include "open62541/plugin/securitypolicy.h"
#include "open62541/plugin/accesscontrol.h"
#include "open62541/plugin/accesscontrol_default.h"
#include "open62541/plugin/pki_default.h"
#include "open62541/plugin/securitypolicy_default.h"
#endif
#undef UA_LOGLEVEL
#define UA_LOGLEVEL UA_LOGLEVEL_ORIG
//
#if UA_MULTITHREADING >= 100
// Sleep is function call in wxWidgets
#include <pthread.h>
#undef Sleep
#endif
//
#include <open62541cpp/open62541cpp_trace.h>
//
#include <string>
#include <stdint.h>
#if defined(__GNUC__)
#include <error.h>
#endif
//
#include <map>
#include <vector>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <functional>
#include <typeinfo>
#include <open62541cpp/propertytree.h>
#include <open62541cpp/objects/UaBaseTypeTemplate.h>
#include <open62541cpp/objects/String.h>
#include <open62541cpp/objects/ByteString.h>
#include <open62541cpp/objects/ArrayTemplate.h>
#include <open62541cpp/objects/NodeId.h>
#include <open62541cpp/objects/QualifiedName.h>
#include <boost/any.hpp>
//
// Open 62541 has quasi new-delete and copy operators for each object type
// define wrappers for Open 62541 objects
//
// With Microsoft Windows watch out for class export problems
// Sometimes templates have to include UA_EXPORT other times not
// If the template is typedef'ed do not export
// If the template is the base of a class it is exported
//
namespace Open62541 {
// non-heap allocation - no delete
// std::string      -> UA_String
UA_String toUA_String(const std::string& str);

// std::string   -> UA_String
void fromStdString(const std::string& in, UA_String& out);

// UA_ByteString -> std::string
inline std::string fromByteString(const UA_ByteString& uaByte) 
{ 
    return std::string((const char*)uaByte.data, uaByte.length); 
}

// UA_String     -> std::string
inline std::string toString(const UA_String& str) 
{ 
    return std::string((const char*)(str.data), str.length); 
}

// UA_Variant    -> std::string
std::string variantToString(const UA_Variant& variant);

// UA_StatusCode -> std::string
inline std::string toString(UA_StatusCode code) { return std::string(UA_StatusCode_name(code)); }

// UA_DateTime   -> std::string
std::string  timestampToString(UA_DateTime date);

// UA_NodeId     -> std::string
UA_EXPORT std::string toString(const UA_NodeId& node);

inline void printLastError(UA_StatusCode code, std::iostream& os) {
    os << UA_StatusCode_name(code) ;
}

// Prints status only if not Good
#define UAPRINTLASTERROR(c) {if(c != UA_STATUSCODE_GOOD) std::cerr << __FUNCTION__ << ":" << __LINE__ << ":" << UA_StatusCode_name(c) << std::endl;}


/**
 * A mask specifying the class of the node.
 * Alias for UA_NodeClass
 * @see UA_NodeClass in open62541.h
 */
typedef UA_NodeClass NodeClass;

typedef std::vector<std::string> Path;

/*!
    \brief VariantList
*/
typedef std::vector<UA_Variant> VariantList;  // shallow copied

class UA_EXPORT ClientSubscription;
class UA_EXPORT MonitoredItem;
class UA_EXPORT Server;
class UA_EXPORT Client;
class UA_EXPORT ServerRepeatedCallback;

//TODO figure out where is the implementation
// 
// UA_DataValue  -> std::string
std::string dataValueToString(const UA_DataValue& value);

} // namespace Open62541
#endif // OPEN62541OBJECTS_H
