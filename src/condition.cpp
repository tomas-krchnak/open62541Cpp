
#include <open62541cpp/condition.h>
#include <open62541cpp/open62541server.h>
#ifdef UA_ENABLE_SUBSCRIPTIONS_ALARMS_CONDITIONS
namespace Open62541 {

/*!
 * \brief Condition::Condition
 * \param s
 * \param c
 * \param src
 */
Condition::Condition(Server& s, const NodeId& c, const NodeId& src)
    : _server(s)
    , _condition(c)
    , _conditionSource(src)
{
}

/*!
 * \brief Condition::~Condition
 */
Condition::~Condition()
{
    _lastError = UA_Server_deleteCondition(_server.server(), _condition, _conditionSource);
}

/* Set the value of condition field.
 *
 * @param server The server object
 * @param condition The NodeId of the node representation of the Condition Instance
 * @param value Variant Value to be written to the Field
 * @param fieldName Name of the Field in which the value should be written
 * @return The StatusCode of the UA_Server_setConditionField method*/
bool Condition::setConditionField(const Variant& v, const std::string& name)
{
    QualifiedName qn(_condition.nameSpaceIndex(), name);
    _lastError = UA_Server_setConditionField(_server.server(), _condition, v, qn);
    return lastOK();
}
/*!
 * \brief Condition::setConditionVariableFieldProperty
 * \param value
 * \param variableFieldName
 * \param variablePropertyName
 * \return
 */
bool Condition::setConditionVariableFieldProperty(const Variant& value,
                                                             const std::string& variableFieldName,
                                                             const std::string& variablePropertyName)
{

    QualifiedName fn(_condition.nameSpaceIndex(), variableFieldName);
    QualifiedName pn(_condition.nameSpaceIndex(), variablePropertyName);
    _lastError = UA_Server_setConditionVariableFieldProperty(_server.server(), _condition, value, fn, pn);
    return lastOK();
}
/*!
 * \brief Condition::triggerConditionEvent
 * \param outEventId
 * \return
 */
bool Condition::triggerConditionEvent(const std::string& outEventId)
{
    ByteString b(outEventId);
    _lastError = UA_Server_triggerConditionEvent(_server.server(), _condition, _conditionSource, b);
    return lastOK();
}

/*!
 * \brief Condition::addConditionOptionalField
 * \param conditionType
 * \param fieldName
 * \param outOptionalVariable
 * \return
 */
bool Condition::addConditionOptionalField(const NodeId& conditionType,
                                                     const std::string& fieldName,
                                                     NodeId& outOptionalVariable)
{
    QualifiedName fn(_condition.nameSpaceIndex(), fieldName);
    _lastError =
        UA_Server_addConditionOptionalField(_server.server(), _condition, conditionType, fn, outOptionalVariable);
    return lastOK();
}

UA_StatusCode Condition::twoStateVariableChangeEnabledStateCallback(UA_Server* server,
                                                                               const UA_NodeId* condition)
{
    Server* s = Server::findServer(server);
    if (s) {
        ConditionPtr& c = s->findCondition(condition);
        if (c) {
            if (c->enteringEnabledState())
                return UA_STATUSCODE_GOOD;
        }
    }
    return UA_StatusCode(-1);
}
UA_StatusCode Condition::twoStateVariableChangeAckedStateCallback(UA_Server* server,
                                                                             const UA_NodeId* condition)
{
    Server* s = Server::findServer(server);
    if (s) {
        ConditionPtr& c = s->findCondition(condition);
        if (c) {
            if (c->enteringAckedState())
                return UA_STATUSCODE_GOOD;
        }
    }
    return UA_StatusCode(-1);
}
UA_StatusCode Condition::twoStateVariableChangeConfirmedStateCallback(UA_Server* server,
                                                                                 const UA_NodeId* condition)
{
    Server* s = Server::findServer(server);
    if (s) {
        ConditionPtr& c = s->findCondition(condition);
        if (c) {
            if (c->enteringConfirmedState())
                return UA_STATUSCODE_GOOD;
        }
    }
    return UA_StatusCode(-1);
}
UA_StatusCode Condition::twoStateVariableChangeActiveStateCallback(UA_Server* server,
                                                                              const UA_NodeId* condition)
{
    Server* s = Server::findServer(server);
    if (s) {
        ConditionPtr& c = s->findCondition(condition);
        if (c) {
            if (c->enteringActiveState())
                return UA_STATUSCODE_GOOD;
        }
    }
    return UA_StatusCode(-1);
}

/*!
 * \brief Condition::setCallback
 * \param callbackType
 * \param removeBranch
 */
bool Condition::setCallback(UA_TwoStateVariableCallbackType callbackType, bool removeBranch)
{
    switch (callbackType) {
        case UA_ENTERING_ENABLEDSTATE:
            _lastError =
                UA_Server_setConditionTwoStateVariableCallback(_server.server(),
                                                               _condition,
                                                               _conditionSource,
                                                               (UA_Boolean)removeBranch,
                                                               Condition::twoStateVariableChangeEnabledStateCallback,
                                                               callbackType);
            break;
        case UA_ENTERING_ACKEDSTATE:
            _lastError =
                UA_Server_setConditionTwoStateVariableCallback(_server.server(),
                                                               _condition,
                                                               _conditionSource,
                                                               (UA_Boolean)removeBranch,
                                                               Condition::twoStateVariableChangeAckedStateCallback,
                                                               callbackType);
            break;
        case UA_ENTERING_CONFIRMEDSTATE:
            _lastError =
                UA_Server_setConditionTwoStateVariableCallback(_server.server(),
                                                               _condition,
                                                               _conditionSource,
                                                               (UA_Boolean)removeBranch,
                                                               Condition::twoStateVariableChangeConfirmedStateCallback,
                                                               callbackType);
            break;
        case UA_ENTERING_ACTIVESTATE:
            _lastError =
                UA_Server_setConditionTwoStateVariableCallback(_server.server(),
                                                               _condition,
                                                               _conditionSource,
                                                               (UA_Boolean)removeBranch,
                                                               Condition::twoStateVariableChangeActiveStateCallback,
                                                               callbackType);
            break;
        default:
            break;
    }
    return lastOK();
}
}
#endif
