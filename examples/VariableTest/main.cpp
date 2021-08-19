#include <iostream>
#define  UA_TRACE_OBJ
#include <open62541cpp/open62541objects.h>
using namespace std;

int main(int argc, char* argv[])
{
    {
        namespace opc = Open62541;
        cout << "Variable Test" << endl;
        opc::NodeId A(1, "Node A");
        opc::NodeId B(1, "Node B");
        opc::NodeId C(1, "Node C");
        cout << "At Start: " << endl;
        cout << " A = " << opc::toString(A)
             << " B = " << opc::toString(B)
             << " C = " << opc::toString(C) << endl;

        cout << "Assign A to C" << endl;
        C = A;
        cout << "After Assign: " << endl;
        cout << " A = " << opc::toString(A)
             << " B = " << opc::toString(B)
             << " C = " << opc::toString(C) << endl;

        cout << "Assigning C types Test" << endl;

        // these should be explicitly deleted using UA_NodeId_deletemembers
        UA_NodeId x = UA_NODEID_NUMERIC(1,1234);
        UA_NodeId y = UA_NODEID_NUMERIC(1,4567);
        UA_NodeId z = UA_NODEID_NUMERIC(1,9876);

        opc::NodeId D(x); // take copy and own
        opc::NodeId E = y;
        opc::NodeId F{z};
        cout << " D = " << opc::toString(D)
             << " E = " << opc::toString(E)
             << " F = " << opc::toString(F) << endl;

        cout << "Expect Final Delete of Z" << endl;
        F = D;
        cout << "Report D,E,F" <<endl;

        cout << " D = " << opc::toString(D)
             << " E = " << opc::toString(E)
             << " F = " << opc::toString(F) << endl;

        cout << "End of scope" << endl;
    }

    cout << "Exited test scope" << endl;

    return 0;
}
