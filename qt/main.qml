import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2
import "json_parse.js" as JS

Window {
  id: ossquestion
  width: 1000
  height: 650
  visible: true
  color: "lightgray"
  title: qsTr("OSSLZU-2016")

  Component.onCompleted: JS.load();

  ListView {
    id: qList
    anchors.leftMargin: 80
    anchors.rightMargin: 80
    boundsBehavior: Flickable.DragAndOvershootBounds
    layoutDirection: Qt.LeftToRight
    keyNavigationWraps: false
    anchors.fill: parent
    clip: false
    orientation: ListView.Vertical
    flickableDirection: Flickable.VerticalFlick
    header: headerComponent
    footer: footerComponent
    focus: true
    width: parent.width
    cacheBuffer: (180 * 25 + 200)
    property int score: 0
    model: ListModel { id: listModel }

    delegate: Item {
      x: 0
      y: 0
      width: parent.width
      height: 180
      Column {
        id: qItem
        width: parent.width
        height: 170
        spacing: 0

        Text {
          id: questions
          width: parent.width
          height: 50
          wrapMode: Text.WordWrap
          text: qsTr((index + 1) + "." + question)
          font.pixelSize: 15
        }

        RadioButton {
          id: optionA
          width: parent.width
          height: 30
          clip: true
          scale: 1
          enabled: true
          text: opa
          onCheckedChanged: JS.check(optionA, answer)
        }

        RadioButton {
          id: optionB
          width: parent.width
          height: 30
          clip: true
          scale: 1
          enabled: true
          text: opb
          onCheckedChanged: JS.check(optionB, answer)
        }

        RadioButton {
          id: optionC
          width: parent.width
          height: 30
          clip: true
          scale: 1
          enabled: true
          text: opc
          onCheckedChanged: JS.check(optionC, answer)
        }

        RadioButton {
          id: optionD
          width: parent.width
          height: 30
          clip: true
          scale: 1
          enabled: true
          text: opd
          onCheckedChanged: JS.check(optionD, answer)
        }
      }
    }
  }

  Component {
    id: headerComponent
    Rectangle {
      width: parent.width; height: 100
      color: ossquestion.color
      Text {
        anchors.centerIn: parent
        text: qsTr("兰州大学开源社区-2016")
        font.pixelSize: 50
      }
      Text {
        text: qsTr("每题 4 分，共 25 题<br><br>Made by 杨壬")
        font.pixelSize: 15
      }
    }
  }

  Component {
    id: footerComponent
    Rectangle {
      width: parent.width; height: 100
      color: ossquestion.color
      Button {
        id: submit
        y: 10
        width: parent.width
        height: parent.height - 20

        Text {
          anchors.centerIn: parent
          text: qsTr("点这里提交")
          font.pixelSize: 40
        }
        onClicked: JS.submit();
      }
    }
  }

  MessageDialog {
    id: scoreBox
    title: "Your Score"
    onAccepted: JS.flush()
  }

  MessageDialog {
    id: veryhighscore
    title: "66666666"
    onAccepted: JS.flush()
  }

  /*Dialog {
    visible: true
    title: "Input questions URL"

    contentItem: Rectangle {
      color: "lightskyblue"
      implicitHeight: 100
      implicitWidth: 400
      TextInput {
        text: qsTr("text")
        cursorVisible: true
      }
    }
  }*/

  MessageDialog {
    id: testDialog
    title: "测试用"
  }
}
