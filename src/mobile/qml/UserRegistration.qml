import QtQuick 2.0
import QtQuick.Controls 1.0
import mplane 1.0
import "android"

Login {
    id: root

    title: "Register user"
    subtitle: "blah ..."

    request: UserRegisterRequest {
    }

    buttonTitle: qsTr("Register")
}
