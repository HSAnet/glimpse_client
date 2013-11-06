import QtQuick 2.0
import QtQuick.Controls 1.0 as Controls

Controls.StackView {
    invalidItemReplacement: Component {
        Label {
            width: parent.width
            height: parent.height
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
    }
}
