var picture = document.getElementById("stream");
var canvasImg = document.getElementById("canvasImg");
var loadingIcon = document.getElementById("loading");
var resMap = new Map();
var width = 0;
var height = 0;
setResMap();
//initCanvasHtml();

async function getInputAddress() {
  let previoushttpRequest = localStorage.getItem('ipAddress');
  let streamState = localStorage.getItem('streamState');
  if (streamState !== '1') {
    var inputAddress = prompt('ip address from arduino');
    var convertedHttpReq = "http://" + inputAddress + "/";
    localStorage.setItem('ipAddress', convertedHttpReq);
    return convertedHttpReq;
  }
  return previoushttpRequest;
}

async function getBackendAddress(){
  let previousBackend = localStorage.getItem('backend');
  let backendState = localStorage.getItem('backendState');
  if (backendState !== '1'){
    var backendAddress = prompt('backend address from api.js');
    var convertedBackReq = "http://" + backendAddress + "/api/getResult";
    localStorage.setItem('backend', convertedBackReq);
    return convertedBackReq;
  }
  return previousBackend;

}

function checkStream(url) {
  showLoadingIcon();
  picture.src = url;
  return new Promise((resolve) => {
    picture.onload = () => {
      width = picture.clientWidth;
      height = picture.clientHeight;
      displayPictureRes(width, height);
      setCanvasRes(width, height);
      drawCanvasFrame();
      drawCanvasInference();
      localStorage.setItem('streamState', '1');
      resolve(true);
    };
    picture.onerror = () => {
      alert("check if the ip address is valid or if CORS is enabled")
      localStorage.setItem('streamState', '0');
      resolve(false);
    }
  }
  );
}

function checkBackend(url){
  return new Promise((resolve) =>{
    fetch(url).then((response)=>{
      console.log(response);
      localStorage.setItem('backendState', '1');
      resolve(true);
    }).catch(err => {
      console.log(err);
      alert("check if the ip address from api.js is valid or if the file is running");
      localStorage.setItem('backendState', '0');
      resolve(true);
    })
  });
}

async function fetchStream() {
  for (let i = 0; i < 5; i++) {
    const inputAddress = await getInputAddress();
    await checkStream(inputAddress).then(console.log);
    await new Promise(resolve => setTimeout(resolve, 200));
    let streamState = localStorage.getItem('streamState');
    if (streamState == '1') {
      removeLoadingIcon();
      break;
    }
  }
}

async function fetchBackend(){
  for(let i = 0; i < 5; i++){
    const backendAddress = await getBackendAddress();
    await checkBackend(backendAddress);
    await new Promise(resolve => setTimeout(resolve, 200));
    let backendState = localStorage.getItem('backendState');
    if(backendState == '1'){
      break;
    }

  }
}
fetchStream();
fetchBackend();

var classLabel = document.getElementById("class");
var captureButton = document.getElementById("capture");
var clearButton = document.getElementById("clear");
var brightness = document.getElementById("brightness");
var stopButton = document.getElementById("stop");
var downloadButton = document.getElementById("download");
var resolution = document.getElementById("resolution");
resolution.addEventListener("change", changeResolution);

var state = 0;
var galleryDict = new Map();
let index = 0;
let colorArr = ["green", "blue", "red", "black", "orange",
  "purple", "pink", "gray"];

//for hiding/displaying settings  
var setting = document.getElementById('setting');
var settings_contents = document.getElementById("setting-contents");
var arrow = document.getElementById("arrow");
var settingHide = true;


var inputInstance = document.getElementById('instance');
inputInstance.value = 30;

var brightness = document.getElementById("brightness");
brightness.addEventListener("change", changeBrightness);

var contrast = document.getElementById("contrast");
contrast.addEventListener("change", changeContrast);

var saturation = document.getElementById("saturation");
saturation.addEventListener("change", changeSaturation);

var whiteball = document.getElementById("whiteball");
whiteball.addEventListener("change", changeWhiteball);

var awb = document.getElementById("awb");
awb.addEventListener("change", changeAwb);

var ae = document.getElementById("ae");
ae.addEventListener("change", changeAe);


var aec2 = document.getElementById("aec2");
aec2.addEventListener("change", changeAec2);
aec2.value = 1;

var gain = document.getElementById("gain");
gain.addEventListener("change", changeGain);

var agc = document.getElementById("agc");
agc.addEventListener("change", changeAgc);

var gainCeling = document.getElementById("gainCeling");
gainCeling.addEventListener("change", changeGainCeiling);

var rawGma = document.getElementById("rawGma");
rawGma.addEventListener("change", changeRawGma);
rawGma.value = 1;

var effect = document.getElementById("effect");
effect.addEventListener("change", changeEffect);

var mode = document.getElementById("mode");
mode.addEventListener("change", changeMode);


resetAllCameraSetting();
updateImageNo();
document.getElementById("galleryImg").style.visibility = "hidden";
document.getElementById('download-delete').style.visibility = 'hidden';
document.getElementById('stop').style.visibility = 'hidden';


async function stopStream() {
  picture.removeAttribute("src");
  canvasImg.style.visibility = "hidden";
  await new Promise(resolve => setTimeout(resolve, 1000));
}

async function drawCanvasFrame() {
  canvasImg.style.visibility = "visible";
  const ctx = canvasImg.getContext('2d');
  while (true) {
    ctx.drawImage(picture, 0, 0, width, height);
    await new Promise(resolve => setTimeout(resolve, 120));
  }
};

async function displayCanvas(picIndex) {
  const displayCanvas = document.createElement("canvas");
  var WidthRatio = Math.ceil((width/height) * 240);
  displayCanvas.id = "cap" + picIndex.toString();
  displayCanvas.setAttribute("width", WidthRatio + "px");
  displayCanvas.setAttribute("height", "240px");
  displayCanvas.style.padding = "10px"
  displayCanvas.style.zIndex = "1";
  const newDiv = createPictureDiv(picIndex);
  const deleteButton = createDeleteButton(picIndex);
  document.getElementById("galleryImg").appendChild(newDiv);
  newDiv.appendChild(displayCanvas);
  newDiv.appendChild(deleteButton);
  const labelImg = document.getElementById("canvasImg");
  displayCanvas.getContext('2d').drawImage(labelImg, 0, 0, WidthRatio, 240);
  document.getElementById("download-delete").style.visibility = "visible";
  document.getElementById("galleryImg").style.visibility = "visible";
}

function createPictureDiv(picIndex) {
  const newDiv = document.createElement("div");
  newDiv.id = "imgDiv" + picIndex.toString();
  newDiv.style.display = "flex";
  newDiv.style.justifyContent = "center"
  newDiv.style.width = "350px";
  newDiv.style.height = "240px"
  newDiv.style.color = "red";
  return newDiv;
}

function createDeleteButton(picIndex) {
  const deleteButton = document.createElement("button");
  deleteButton.id = picIndex.toString();
  deleteButton.style.zIndex = "2";
  deleteButton.style.position = "relative";
  deleteButton.style.fontSize = "17px";
  deleteButton.style.fontWeight = "700";
  deleteButton.style.left = "-20px";
  deleteButton.style.width = "22px";
  deleteButton.style.height = "22px"
  deleteButton.style.border = "none";
  deleteButton.style.borderRadius = "50%";
  deleteButton.style.backgroundColor = "red";
  deleteButton.style.color = "bisque";
  deleteButton.style.cursor = "pointer";
  deleteButton.innerText = "-";
  deleteButton.setAttribute("onclick", "deleteButton(this.id)");
  return deleteButton;
}

async function savePicture(picIndex) {
  const hiddenCanvas = document.createElement("canvas");
  hiddenCanvas.id = "saved" + picIndex;
  hiddenCanvas.setAttribute("width", width + "px");
  hiddenCanvas.setAttribute("height", height + "px");
  hiddenCanvas.getContext('2d').drawImage(picture, 0, 0, width, height);
  let image_data_url = hiddenCanvas.toDataURL('image' + index - 1 + '/jpg');
  galleryDict.set("saved" + picIndex, image_data_url);
  await updateImageNo();
}

function getInstance() {
  if (!Number.isInteger(inputInstance.value)) {
    if (inputInstance < 1) {
      inputInstance.value = Math.round(inputInstance.value + 1);
    }
    else {
      inputInstance.value = Math.round(inputInstance.value);
    }
  }
  return inputInstance.value;
}

function decrementInstance(){
  var currentInstance = getInstance();
  inputInstance.value = currentInstance - 1;
}

async function updateImageNo (){
  var imageNo = document.getElementById("image_no");
  imageNo.value = galleryDict.size;
}
function deleteButton(pictureId) {
  document.getElementById("imgDiv" + pictureId).remove();
  galleryDict.delete("saved" + pictureId);
  updateImageNo();
  if (galleryDict.size == 0) {
    document.getElementById("galleryImg").style.visibility = "hidden";
    document.getElementById("download-delete").style.visibility = "hidden";
  }
}

async function chageOtherSettings(setting) {
  if (setting !== "bright") {
    brightness.value = 0;
  }
  if (setting !== "contrast") {
    contrast.value = 0;
  }
  if (setting !== "saturation") {
    saturation.value = 0;
  }
  if (setting !== "ae") {
    ae.value = 0;
  }
  else if (setting !== "effect") {
    effect.value = 0;
  }
}

function resetAllCameraSetting() {
  brightness.value = 0;
  contrast.value = 0;
  saturation.value = 0;
  ae.value = 0;
  effect.value = 0;
  mode.value = 0;
}

async function changeBrightness() {
  var selectedBrightness = brightness.options[brightness.selectedIndex].value;
  var params = "bright=" + selectedBrightness
  await ChangeSetting(params);
  await chageOtherSettings("bright");
}

async function changeContrast() {
  var selectedContrast = contrast.options[contrast.selectedIndex].value;
  var params = "contrast=" + selectedContrast;
  await ChangeSetting(params);
  await chageOtherSettings("contrast");
}

async function changeSaturation() {
  var selectedSaturation = saturation.options[saturation.selectedIndex].value;
  var params = "saturation=" + selectedSaturation;
  await ChangeSetting(params);
  await chageOtherSettings("saturation");
}

async function changeWhiteball(){
  var selectedWhiteball = whiteball.options[whiteball.selectedIndex].value;
  var params = "whiteball=" + selectedWhiteball;
  await ChangeSetting(params);
}

async function changeAwb(){
  var selectedAwb = awb.options[awb.selectedIndex].value;
  var params = "awb=" + selectedAwb;
  await ChangeSetting(params); 
}

async function changeAe() {
  var selectedAe = ae.options[ae.selectedIndex].value;
  var params = "ae=" + selectedAe;
  await ChangeSetting(params);
  await chageOtherSettings("ae");
}

async function changeExposure(){
  var selectedExposure = exposure.options[exposure.selectedIndex].value;
  var params = "exposure=" + selectedExposure;
  await ChangeSetting(params);
}

async function changeAec2(){
  var selectedAec2 = aec2.options[aec2.selectedIndex].value;
  var params = "aec2=" + selectedAec2;
  await ChangeSetting(params);
}

async function changeGain(){
  var selectedGain = gain.options[gain.selectedIndex].value;
  var params = "gain=" + selectedGain;
  await ChangeSetting(params);
}

async function changeAgc(){
  var selectedAgc = agc.options[agc.selectedIndex].value;
  var params = "agc=" + selectedAgc;
  await ChangeSetting(params);
}

async function changeGainCeiling(){
  var selectedGainCeiling = gainCeling.options[gainCeling.selectedIndex].value;
  var params = "gainCeiling=" + selectedGainCeiling;
  await ChangeSetting(params);
}

async function changeRawGma(){
  var selectedRawGma = rawGma.options[rawGma.selectedIndex].value;
  var params = "rawGma=" + selectedRawGma;
  await ChangeSetting(params);
}

async function changeEffect() {
  var selectedEffect = effect.options[effect.selectedIndex].value;
  var params = "effect=" + selectedEffect;
  await ChangeSetting(params);
  await chageOtherSettings("effect");
}

async function changeMode() {
  var selectedMode = mode.options[mode.selectedIndex].value;
  var params = "mode=" + selectedMode;
  await ChangeSetting(params);
}

async function changeResolution() {
  var selectedRes = resolution.options[resolution.selectedIndex].value;
  var params = "resolution=" + selectedRes;
  await ChangeSetting(params);
  resetAllSetting();
}

setting.addEventListener("click", function (){
  console.log(settingHide);
  if (settingHide == true){
    settings_contents.style.visibility = "hidden";
    arrow.src = "icons/caret-up.png";
    settingHide = false;
  }else if (settingHide == false){
    settingHide = true;
    arrow.src = "icons/caret-down.png";
    settings_contents.style.visibility = "visible";
  }
})

captureButton.addEventListener("click", async function () {
  console.log("capture was clicked");
  document.getElementById('capture').style.visibility = 'hidden';
  document.getElementById('stop').style.visibility = 'visible';
  state = 1;
  var instance = getInstance();
  while (instance != 0) {
    if (state === 0) {
      break;
    }
    instance = getInstance();
    console.log(instance);
    await new Promise(resolve => setTimeout(resolve, 120));
  }
  state = 0;
  document.getElementById('capture').style.visibility = 'visible';
  document.getElementById('stop').style.visibility = 'hidden';
})

clearButton.addEventListener("click", function () {
  const element = document.getElementById("galleryImg");
  const hiddenCan = document.getElementById("savedImg");
  while (element.firstChild) {
    element.removeChild(element.firstChild);
  }
  while (hiddenCan.firstChild) {
    hiddenCan.removeChild(hiddenCan.firstChild);
  }

  galleryDict.clear();
  updateImageNo();
  document.getElementById("galleryImg").style.visibility = "hidden";
  document.getElementById("download-delete").style.visibility = "hidden";
})

stopButton.addEventListener("click", async function () {
  document.getElementById('capture').style.visibility = 'visible';
  document.getElementById('stop').style.visibility = 'hidden';
  state = 0;
})

async function ChangeSetting(params) {
  await stopStream();
  let previoushttpRequest = localStorage.getItem('ipAddress');
  fetch(previoushttpRequest + 'setting?' + params)
    .then(response => {
      if (!response.ok) {
        throw new Error('Network response was not ok');
      }
      //return response.json();
    })
    .then(data => {
      console.log(data);
      fetchStream();
    })
}

downloadButton.addEventListener("click", async function () {
  var downloadLink = document.createElement("a");
  const zip = new JSZip();
  if (galleryDict.size == 0) {
    alert("No image found!!");
    return;
  }
  var count = 0;
  for (const value of galleryDict.values()) {
    let data = value.substr(value.indexOf(",") + 1)
    count++;
    zip.file(count + ".jpg", data, { base64: true })
  }

  const zipFile = await zip.generateAsync({ type: 'blob' });
  downloadLink.download = "images_of_" + '.zip';

  const url = URL.createObjectURL(zipFile);
  downloadLink.href = url;
  downloadLink.style.display = 'none';
  document.body.appendChild(downloadLink);
  downloadLink.click();
  downloadLink.remove();
  URL.revokeObjectURL(url);
})

async function getInference() {
  const backendReq = localStorage.getItem("backend");
  const response = await fetch(backendReq);
  const data = await response.json();
  return data;
}

async function drawResult(keys, data) {
  if (keys[0] !== "name") {
    index++;
    const ctx = canvasImg.getContext('2d');
    for (let i = 0; i < keys.length; i++) {
      const confidence = Math.round(data[keys[i]][0]);
      const result = keys[i] + " " + confidence;
      const x = data[keys[i]][1];
      const y = data[keys[i]][2];
      ctx.font = 'bold 15px Arial';
      ctx.fillStyle = colorArr[i];
      ctx.fillRect(x * 2.5, (y - 10) * 2.5, result.length + 52, 18);
      ctx.fillStyle = "white";
      ctx.fillText(result, x * 2.5, (y - 4) * 2.5);
      ctx.beginPath();
      //console.log(keys[i]);
      //console.log(data);
      ctx.strokeStyle = colorArr[i];
      ctx.arc(x * 2.5, y * 2.5, 8, 0, 2 * Math.PI);
      ctx.lineWidth = 1.5;
      ctx.stroke();
    }
    var currentInstance = getInstance();
    if (state === 1 && currentInstance > 0){
      await displayCanvas(index);
      await savePicture(index);
      decrementInstance();
    }
  }
}
async function getKey(data) {
  var inferences = [];
  for (const key in data) {
    if (key === "name") {
      inferences.push(key);
      return inferences;
    }
    inferences.push(key);
  }
  return inferences;
}

async function drawCanvasInference() {
  while (true) {
    const inferences = await getInference();
    var keys = await getKey(inferences);
    await drawResult(keys, inferences);
    await new Promise(resolve => setTimeout(resolve, 120));
  }
}

function setResMap() {
  //resMap for selecting correct resolution index given by pciture width and height
  resMap.set("160120", 0);
  resMap.set("240240", 1);
  resMap.set("320240", 2)
  resMap.set("640480", 3);
  resMap.set("800600", 4);
  resMap.set("1024768", 5);
  resMap.set("12801024", 6);
  resMap.set("16001200", 7);
}


function displayPictureRes(picWidth, picHeight) {
  width = picWidth;
  height = picHeight;
  var mapKey = width.toString() + height.toString();
  resolution.selectedIndex = resMap.get(mapKey);
}

function showLoadingIcon(){
  loadingIcon.src = "./loading.gif";
  loadingIcon.setAttribute("width", "120");
  loadingIcon.setAttribute("height", "120");
  loadingIcon.style.visibility = "visible";
}

function removeLoadingIcon(){
  loadingIcon.removeAttribute('src');
  loadingIcon.style.visibility = 'hidden';
}

function setCanvasRes(picWidth, picHeight){
  canvasImg.setAttribute("width", picWidth.toString());
  canvasImg.setAttribute("height", picHeight.toString());
}

var instanceLabel = document.getElementById("instanceLabel");
var brightLabel = document.getElementById("brightLabel");
var contrastLabel = document.getElementById("contrastLabel");
var saturationLabel = document.getElementById("saturationLabel");
var whiteBallLabel = document.getElementById("whiteballLabel");
var awbLabel = document.getElementById("awbLabel");
var aeLabel = document.getElementById("aeLabel");
var exposureLabel = document.getElementById("exposureLabel");
var aec2Label = document.getElementById("aec2Label");
var gainLabel = document.getElementById("gainLabel");
var agcLabel = document.getElementById("agcLabel");
var gainCelingLabel = document.getElementById("gainCelingLabel");
var rawGmaLabel = document.getElementById("rawGmaLabel");
var effectLabel = document.getElementById("effectLabel");
var modeLabel = document.getElementById("modeLabel");

const en =
{
  "instances": "Instances",
  "brightness": "Brightness",
  "contrast": "Contrast",
  "saturation": "Saturation",
  "whiteBall" : "White Ball",
  "awb" : "Auto Whiteball",
  "ae_level": "Auto Exposure",
  "aec2" : "Auto Exposure Control",
  "gain" : "Gain Control",
  "agc" : "Auto Gain Control",
  "gainCeling" : "Gain Ceiling",
  "rawGma" : "Raw GMA",
  "effects": "Effect",
  "mode": "Mode"
}


const th =
{
  "instances": "จำนวนรูป",
  "brightness": "ความสว่าง",
  "contrast": "ความต่างระดับสี",
  "saturation": "ความอิ่มสี",
  "whiteBall" : "สมดุลแสงสีขาว",
  "awb" : "สมดุลแสงสีขาวแบบออโต้",
  "ae_level": "การเปิดรับแสงแบบออโต้",
  "aec2" : "การคุมแสงแบบออโต้",
  "gain" : "ความไวต่อแสง",
  "agc" : "ความไวต่อแสงแบบออโต้",
  "gainCeling" : "ระดับค่าของความไวต่อแสง",
  "rawGma" : "ความสว่างกล้องแบบออโต้",
  "effects": "เอฟเฟคกล้อง",
  "mode": "โหมดกล้อง"
}

function changeLanguage(lang){
  instanceLabel.innerHTML = lang.instances;
  brightLabel.innerHTML = lang.brightness;
  contrastLabel.innerHTML = lang.contrast;
  saturationLabel.innerHTML = lang.saturation;
  whiteBallLabel.innerHTML = lang.whiteBall;
  awbLabel.innerHTML = lang.awb;
  aeLabel.innerHTML = lang.ae_level;
  aec2Label.innerHTML = lang.aec2;
  gainLabel.innerHTML = lang.gain;
  agcLabel.innerHTML = lang.agc;
  gainCelingLabel.innerHTML = lang.gainCeling;
  rawGmaLabel.innerHTML = lang.rawGma;
  effectLabel.innerHTML = lang.effects;
  modeLabel.innerText = lang.mode;
}

var englishSelect = document.getElementById("englishSelect");
var thaiSelect = document.getElementById("thaiSelect");

function getLanguage(){
  var getLang = localStorage.getItem("lang");
  if (getLang == "en"){
    englishSelect.click();
  }
  else if(getLang = "th"){
    thaiSelect.click();
  }
  else{
    englishSelect.click();
  }
}

englishSelect.addEventListener("click", function () {
  thaiSelect.style.color = "bisque";
  englishSelect.style.color = "blue";
  changeLanguage(en);
  localStorage.setItem("lang", "en");
})

thaiSelect.addEventListener("click", function () {
  thaiSelect.style.color = "blue";
  englishSelect.style.color = "bisque";
  changeLanguage(th);
  localStorage.setItem("lang", "th");
})

getLanguage();