var qjson;

function load() {
  var xhr = new XMLHttpRequest();

  xhr.open("GET", "http://localhost/qjson", true);
  xhr.onreadystatechange = function() {
    if (xhr.readyState == xhr.DONE) {
      if (xhr.status == 200) {
        qjson = eval('(' + xhr.responseText + ')');
        loaded();
      }
    }
  }
  xhr.send();
}

function loaded() {
  var count = new Array();
  var sign = 0;
  while (count.length < 25) {
    sign = 0;
    var index = Math.round(Math.random() * 1000) % 311;
    for (var i = 0; i < count.length; ++i) {
      if (count[i] === index)
        sign = 1;
    }

    if (sign == 0) {
      count.push(index);
      listModel.append({ "question": qjson[index]["Q"],
                         "answer": qjson[index]["answer"],
                         "opa": qjson[index]["select"][0],
                         "opb": qjson[index]["select"][1],
                         "opc": qjson[index]["select"][2],
                         "opd": qjson[index]["select"][3]});
    }
  }
}

function flush() {
  listModel.clear();
  JS.loaded();
  qList.positionViewAtBeginning();
  qList.score = 0;
}

function check(select, answer) {
  if (select.checked && select.text === answer) {
    select.checked = true;
    qList.score += 4;
  } else if (!select.checked && select.text === answer) {
    select.checked = false;
    qList.score -= 4;
  }
}

function submit() {
  var score = qList.score;

  if (score > 92) {
    veryhighscore.text = "得分: " + score + "<br><br>年轻人很有潜力啊！";
    veryhighscore.open();
  } else {
    scoreBox.text = "得分: " + score;
    scoreBox.open();
  }
}
