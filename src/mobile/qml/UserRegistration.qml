import QtQuick 2.0
import mplane 1.0

Login {
    id: root

    title: "Registration"
    subtitle: "blah ..."

    request: UserRegisterRequest {
    }

    buttonTitle: qsTr("Register")
}
