
/*
    Copyright (C) 2017 -  B. J. Hill

    This file is part of open62541 C++ classes. open62541 C++ classes are free software: you can
    redistribute it and/or modify it under the terms of the Mozilla Public
    License v2.0 as stated in the LICENSE file provided with open62541.

    open62541 C++ classes are distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.
*/
#include <open62541cpp/objects/NodeId.h>

namespace Open62541 {
// Standard static nodes
NodeId NodeId::Objects(0, UA_NS0ID_OBJECTSFOLDER);
NodeId NodeId::Server(0, UA_NS0ID_SERVER);
NodeId NodeId::Null(0, 0);
NodeId NodeId::Organizes(0, UA_NS0ID_ORGANIZES);
NodeId NodeId::FolderType(0, UA_NS0ID_FOLDERTYPE);
NodeId NodeId::HasOrderedComponent(0, UA_NS0ID_HASORDEREDCOMPONENT);
NodeId NodeId::BaseObjectType(0, UA_NS0ID_BASEOBJECTTYPE);
NodeId NodeId::HasSubType(0, UA_NS0ID_HASSUBTYPE);
NodeId NodeId::HasModellingRule(0, UA_NS0ID_HASMODELLINGRULE);
NodeId NodeId::ModellingRuleMandatory(0, UA_NS0ID_MODELLINGRULE_MANDATORY);
NodeId NodeId::HasComponent(0, UA_NS0ID_HASCOMPONENT);
NodeId NodeId::HasProperty(0, UA_NS0ID_HASPROPERTY);
NodeId NodeId::BaseDataVariableType(0, UA_NS0ID_BASEDATAVARIABLETYPE);
NodeId NodeId::HasNotifier(0, UA_NS0ID_HASNOTIFIER);
NodeId NodeId::BaseEventType(0, UA_NS0ID_BASEEVENTTYPE);

}  // namespace Open62541
