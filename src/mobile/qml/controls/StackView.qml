import QtQuick 2.0

StackViewCopy {
    invalidItemReplacement: Component {
        Label {
            width: parent.width
            height: parent.height
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
    }
}
