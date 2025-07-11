#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <FS.h>

const char* ssid = "REDE-DRONE";
const char* password = "1212";;

IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
ESP8266WebServer server(80);

const char pagina_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
      <title>PROJETO-DRONE-MARK7-SSTA</title>
      <link rel="stylesheet" href="style.css">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
  </head>
  <body>
      <div class="div-funcoes-especiais">
          <button id="BotaoLigarDesligar" class="botao-ligar-desligar"><h1>ON / OFF</h1></button>
          <button id="BotaoVoltarCasa" class="botao-voltar-casa"><h1>HOME</h1></button>
          <button id="BotaoSoltarCarga" class="botao-soltar-carga"><h1>DEPLOY</h1></button>
          <button id="BotaoDecolar" class="botao-decolar"><h1>DECOLAR</h1></button>
          <button id="BotaoPousar" class="botao-pousar"><h1>POUSAR</h1></button>
      </div>

      <script src="/nipplejs.min.js"></script> <!-- importando a biblioteca nippleJS (serve para criar joysticks)-->
      <div class="div-controles-direcoes">
          <div id="joystickAlturaRotacao" class="joystick-altura-rotacao"></div>
          <div id="joystickDirecoes" class="joystick-direcoes"></div>
      </div>
      <script src="integracao.js" defer></script>
  </body>
  </html>
)rawliteral";

const char style_css[] PROGMEM = R"rawliteral(
    body {
      background-color: #121212;
      margin: 0;
      padding: 0;
  }

  /* Área dos controles inferiores */
  .div-controles-direcoes { 
      display: flex;
      height: 250px;
      width: 100%;
      background-color: #414149;
      position: fixed;
      bottom: 0;
      left: 0;
      align-items: center;
      justify-content: space-around;
      gap: 15px;
      z-index: 10;
  }

  /* Botões de funções especiais */
  .div-funcoes-especiais {
      display: flex;
      flex-wrap: wrap;
      gap: 10px;
      justify-content: center;
      align-items: center;
      background-color: #29292e;
      padding: 10px;
      border-radius: 30px;
      margin: 10px;
  }

  /* Estilização dos botões */
  .botao-ligar-desligar,
  .botao-decolar,
  .botao-pousar,
  .botao-voltar-casa,
  .botao-soltar-carga {
      height: 80px;
      width: 80px;
      background-color: #35353b;
      border-radius: 20%;
      display: flex;
      align-items: center;
      justify-content: center;
      color: white;
      font-size: 12px;
      text-align: center;
  }

  /* Texto dos botões */
  h1 {
      color: rgb(174, 171, 171);
      font-size: 13px;
      margin: 0;
  }

  /* Joysticks */
  .joystick-altura-rotacao, .joystick-direcoes {
      width: 120px;
      height: 120px;
      position: relative;
      border-radius: 50%;
  }

  /* Responsivo para telas pequenas */
  @media (max-width: 420px) {
    .div-controles-direcoes {
      height: 220px;
      gap: 10px;
    }

    .joystick-altura-rotacao, .joystick-direcoes {
      width: 100px;
      height: 100px;
    }

    .botao-ligar-desligar,
    .botao-decolar,
    .botao-pousar,
    .botao-voltar-casa,
    .botao-soltar-carga {
      height: 70px;
      width: 70px;
      font-size: 11px;
    }

    h1 {
      font-size: 11px;
    }
  }


)rawliteral";

const char integracao_js[] PROGMEM = R"rawliteral(
  window.onload = function() {
  var joystickAlturaRotacao = document.getElementById("joystickAlturaRotacao")
  var joystickDirecoes = document.getElementById("joystickDirecoes")
  var botaoLigarDesligar = document.getElementById("BotaoLigarDesligar")
  var botaoVoltarCasa = document.getElementById("BotaoVoltarCasa")
  var botaoSoltarCarga = document.getElementById("BotaoSoltarCarga")
  var botaoDecolar = document.getElementById("BotaoDecolar")
  var botaoPousar = document.getElementById("BotaoPousar")

  let podeEnviarAlturaRotacao = true;
  let podeEnviarDirecoes = true;


  var EstadoJoystickAlturaRotacao = {Direcao: "", Forca: 0}
  var EstadoJoystickDirecoes = {Direcao: "", Forca: 0}

  //configuracao do joystick altura e rotacao
  joystickAlturaRotacao = nipplejs.create({
   zone : joystickAlturaRotacao,
   mode: 'static',
   position: {left: '250px', top: '50%'},
   color: 'gray',
   size: 160
  })

  joystickAlturaRotacao.on('move', function(event, data) {
    if (podeEnviarAlturaRotacao) {
      EstadoJoystickAlturaRotacao.Direcao = data.angle?.degree;
      EstadoJoystickAlturaRotacao.Forca = data.distance;

      fetch('/joystickAlturaRotacao', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({
          EstadoJoystickAlturaRotacao: EstadoJoystickAlturaRotacao
        })
      })
      .then(response => response.text())
      .then(respostaDoServidor => {
        console.log('Resposta do servidor: ', respostaDoServidor)
      })
      .catch(error => {
        console.error('Erro: ', error);
      });

      // Bloqueia envios por 100ms (ajuste conforme quiser)
      podeEnviarAlturaRotacao = false;
      setTimeout(() => {
        podeEnviarAlturaRotacao = true;
      }, 100);
    }
  });


  //configuracao do joystick direcoes
  joystickDirecoes = nipplejs.create({
    zone: joystickDirecoes,
    mode: 'static',
    position: {right: '250px', top: '50%'},
    color: 'gray',
    size: 160
  })

    joystickDirecoes.on('move', function(event, data) {
    if (podeEnviarDirecoes) {
      EstadoJoystickDirecoes.Direcao = data.angle?.degree;
      EstadoJoystickDirecoes.Forca = data.distance;

      fetch('/joystickDirecoes', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({
          EstadoJoystickDirecoes: EstadoJoystickDirecoes
        })
      })
      .then(response => response.text())
      .then(respostaDoServidor => {
        console.log('Resposta do servidor: ', respostaDoServidor)
      })
      .catch(error => {
        console.error('Erro: ', error);
      });

      // Bloqueia envios por 100ms (ajuste conforme quiser)
      podeEnviarDirecoes = false;
      setTimeout(() => {
        podeEnviarDirecoes = true;
      }, 100);
    }
  });


  //endpoints para funcoes do drone
    botaoLigarDesligar.addEventListener("click", function(){
        fetch("/ligarDesligar").then(response => {
          if(response.ok){
            console.log("Comando ligar-desligar enviado")
          }
        });
    });

    botaoDecolar.addEventListener("click", function(){
        fetch("/decolar").then(response => {
            if(response.ok){
                console.log("Comando decolar enviado")
            }
        })
    })

    botaoPousar.addEventListener("click", function() {
        fetch("/pousar").then(response => {
            if(response.ok) {
                console.log("comando pousar enviado")
            }
        })
    })

    botaoVoltarCasa.addEventListener("click", function(){
        fetch("/casa").then(response => {
            if(response.ok) {
                console.log("Comando voltar para casa enviado")
            }
        })
    })

    botaoSoltarCarga.addEventListener("click", function() {
        fetch("/soltarCarga").then(response => {
            if(response.ok) {
                console.log("Comando solcar carga enviado")
            }
        })
    })
  };
)rawliteral";

const char nipple_js[] PROGMEM = R"rawliteral(
  !function(t,i){"object"==typeof exports&&"object"==typeof module?module.exports=i():"function"==typeof define&&define.amd?define("nipplejs",[],i):"object"==typeof exports?exports.nipplejs=i():t.nipplejs=i()}(window,function(){return o={},n.m=e=[function(t,i,e){"use strict";e.r(i);function v(t,i){var e=i.x-t.x,t=i.y-t.y;return Math.sqrt(e*e+t*t)}function b(t){return t*(Math.PI/180)}function n(t){h.has(t)&&clearTimeout(h.get(t)),h.set(t,setTimeout(t,100))}function s(t,i,e){for(var o,n=i.split(/[ ,]+/g),s=0;s<n.length;s+=1)o=n[s],t.addEventListener?t.addEventListener(o,e,!1):t.attachEvent&&t.attachEvent(o,e)}function o(t,i,e){for(var o,n=i.split(/[ ,]+/g),s=0;s<n.length;s+=1)o=n[s],t.removeEventListener?t.removeEventListener(o,e):t.detachEvent&&t.detachEvent(o,e)}function r(t){return t.preventDefault(),t.type.match(/^touch/)?t.changedTouches:t}function d(){return{x:void 0!==window.pageXOffset?window.pageXOffset:(document.documentElement||document.body.parentNode||document.body).scrollLeft,y:void 0!==window.pageYOffset?window.pageYOffset:(document.documentElement||document.body.parentNode||document.body).scrollTop}}function x(t,i){i.top||i.right||i.bottom||i.left?(t.style.top=i.top,t.style.right=i.right,t.style.bottom=i.bottom,t.style.left=i.left):(t.style.left=i.x+"px",t.style.top=i.y+"px")}function p(t,i,e){var o,n=u(t);for(o in n)if(n.hasOwnProperty(o))if("string"==typeof i)n[o]=i+" "+e;else{for(var s="",r=0,d=i.length;r<d;r+=1)s+=i[r]+" "+e+", ";n[o]=s.slice(0,-2)}return n}function a(t,i){for(var e in i)i.hasOwnProperty(e)&&(t[e]=i[e]);return t}function c(t,i){if(t.length)for(var e=0,o=t.length;e<o;e+=1)i(t[e]);else i(t)}var l,h=new Map,u=function(i){var e={};return e[i]="",["webkit","Moz","o"].forEach(function(t){e[t+i.charAt(0).toUpperCase()+i.slice(1)]=""}),e},f=!!("ontouchstart"in window),y=!!window.PointerEvent,m=!!window.MSPointerEvent,e={start:"mousedown",move:"mousemove",end:"mouseup"},g={};function O(){}y?l={start:"pointerdown",move:"pointermove",end:"pointerup, pointercancel"}:m?l={start:"MSPointerDown",move:"MSPointerMove",end:"MSPointerUp"}:f?(l={start:"touchstart",move:"touchmove",end:"touchend, touchcancel"},g=e):l=e,O.prototype.on=function(t,i){var e,o=t.split(/[ ,]+/g);this._handlers_=this._handlers_||{};for(var n=0;n<o.length;n+=1)e=o[n],this._handlers_[e]=this._handlers_[e]||[],this._handlers_[e].push(i);return this},O.prototype.off=function(t,i){return this._handlers_=this._handlers_||{},void 0===t?this._handlers_={}:void 0===i?this._handlers_[t]=null:this._handlers_[t]&&0<=this._handlers_[t].indexOf(i)&&this._handlers_[t].splice(this._handlers_[t].indexOf(i),1),this},O.prototype.trigger=function(t,i){var e,o=this,n=t.split(/[ ,]+/g);o._handlers_=o._handlers_||{};for(var s=0;s<n.length;s+=1)e=n[s],o._handlers_[e]&&o._handlers_[e].length&&o._handlers_[e].forEach(function(t){t.call(o,{type:e,target:o},i)})},O.prototype.config=function(t){this.options=this.defaults||{},t&&(this.options=function(t,i){var e,o={};for(e in t)t.hasOwnProperty(e)&&i.hasOwnProperty(e)?o[e]=i[e]:t.hasOwnProperty(e)&&(o[e]=t[e]);return o}(this.options,t))},O.prototype.bindEvt=function(t,i){var e=this;return e._domHandlers_=e._domHandlers_||{},e._domHandlers_[i]=function(){"function"==typeof e["on"+i]?e["on"+i].apply(e,arguments):console.warn('[WARNING] : Missing "on'+i+'" handler.')},s(t,l[i],e._domHandlers_[i]),g[i]&&s(t,g[i],e._domHandlers_[i]),e},O.prototype.unbindEvt=function(t,i){return this._domHandlers_=this._domHandlers_||{},o(t,l[i],this._domHandlers_[i]),g[i]&&o(t,g[i],this._domHandlers_[i]),delete this._domHandlers_[i],this};e=O;function w(t,i){return this.identifier=i.identifier,this.position=i.position,this.frontPosition=i.frontPosition,this.collection=t,this.defaults={size:100,threshold:.1,color:"white",fadeTime:250,dataOnly:!1,restJoystick:!0,restOpacity:.5,mode:"dynamic",zone:document.body,lockX:!1,lockY:!1,shape:"circle"},this.config(i),"dynamic"===this.options.mode&&(this.options.restOpacity=0),this.id=w.id,w.id+=1,this.buildEl().stylize(),this.instance={el:this.ui.el,on:this.on.bind(this),off:this.off.bind(this),show:this.show.bind(this),hide:this.hide.bind(this),add:this.addToDom.bind(this),remove:this.removeFromDom.bind(this),destroy:this.destroy.bind(this),setPosition:this.setPosition.bind(this),resetDirection:this.resetDirection.bind(this),computeDirection:this.computeDirection.bind(this),trigger:this.trigger.bind(this),position:this.position,frontPosition:this.frontPosition,ui:this.ui,identifier:this.identifier,id:this.id,options:this.options},this.instance}w.prototype=new e,(w.constructor=w).id=0,w.prototype.buildEl=function(t){return this.ui={},this.options.dataOnly||(this.ui.el=document.createElement("div"),this.ui.back=document.createElement("div"),this.ui.front=document.createElement("div"),this.ui.el.className="nipple collection_"+this.collection.id,this.ui.back.className="back",this.ui.front.className="front",this.ui.el.setAttribute("id","nipple_"+this.collection.id+"_"+this.id),this.ui.el.appendChild(this.ui.back),this.ui.el.appendChild(this.ui.front)),this},w.prototype.stylize=function(){if(this.options.dataOnly)return this;var t=this.options.fadeTime+"ms",i=function(){var t,i=u("borderRadius");for(t in i)i.hasOwnProperty(t)&&(i[t]="50%");return i}(),e=p("transition","opacity",t),t={};return t.el={position:"absolute",opacity:this.options.restOpacity,display:"block",zIndex:999},t.back={position:"absolute",display:"block",width:this.options.size+"px",height:this.options.size+"px",marginLeft:-this.options.size/2+"px",marginTop:-this.options.size/2+"px",background:this.options.color,opacity:".5"},t.front={width:this.options.size/2+"px",height:this.options.size/2+"px",position:"absolute",display:"block",marginLeft:-this.options.size/4+"px",marginTop:-this.options.size/4+"px",background:this.options.color,opacity:".5"},a(t.el,e),"circle"===this.options.shape&&a(t.back,i),a(t.front,i),this.applyStyles(t),this},w.prototype.applyStyles=function(t){for(var i in this.ui)if(this.ui.hasOwnProperty(i))for(var e in t[i])this.ui[i].style[e]=t[i][e];return this},w.prototype.addToDom=function(){return this.options.dataOnly||document.body.contains(this.ui.el)||this.options.zone.appendChild(this.ui.el),this},w.prototype.removeFromDom=function(){return this.options.dataOnly||!document.body.contains(this.ui.el)||this.options.zone.removeChild(this.ui.el),this},w.prototype.destroy=function(){clearTimeout(this.removeTimeout),clearTimeout(this.showTimeout),clearTimeout(this.restTimeout),this.trigger("destroyed",this.instance),this.removeFromDom(),this.off()},w.prototype.show=function(t){var i=this;return i.options.dataOnly||(clearTimeout(i.removeTimeout),clearTimeout(i.showTimeout),clearTimeout(i.restTimeout),i.addToDom(),i.restCallback(),setTimeout(function(){i.ui.el.style.opacity=1},0),i.showTimeout=setTimeout(function(){i.trigger("shown",i.instance),"function"==typeof t&&t.call(this)},i.options.fadeTime)),i},w.prototype.hide=function(i){var e=this;return e.options.dataOnly||(e.ui.el.style.opacity=e.options.restOpacity,clearTimeout(e.removeTimeout),clearTimeout(e.showTimeout),clearTimeout(e.restTimeout),e.removeTimeout=setTimeout(function(){var t="dynamic"===e.options.mode?"none":"block";e.ui.el.style.display=t,"function"==typeof i&&i.call(e),e.trigger("hidden",e.instance)},e.options.fadeTime),e.options.restJoystick&&e.setPosition(i,{x:0,y:0})),e},w.prototype.setPosition=function(t,i){var e=this;e.frontPosition={x:i.x,y:i.y};var o=e.options.fadeTime+"ms",i={};i.front=p("transition",["top","left"],o);o={front:{}};o.front={left:e.frontPosition.x+"px",top:e.frontPosition.y+"px"},e.applyStyles(i),e.applyStyles(o),e.restTimeout=setTimeout(function(){"function"==typeof t&&t.call(e),e.restCallback()},e.options.fadeTime)},w.prototype.restCallback=function(){var t={};t.front=p("transition","none",""),this.applyStyles(t),this.trigger("rested",this.instance)},w.prototype.resetDirection=function(){this.direction={x:!1,y:!1,angle:!1}},w.prototype.computeDirection=function(t){var i,e,o,n=t.angle.radian,s=Math.PI/4,r=Math.PI/2;if(s<n&&n<3*s&&!t.lockX?i="up":-s<n&&n<=s&&!t.lockY?i="left":3*-s<n&&n<=-s&&!t.lockX?i="down":t.lockY||(i="right"),t.lockY||(e=-r<n&&n<r?"left":"right"),t.lockX||(o=0<n?"up":"down"),t.force>this.options.threshold){var d,p={};for(d in this.direction)this.direction.hasOwnProperty(d)&&(p[d]=this.direction[d]);var a={};for(d in this.direction={x:e,y:o,angle:i},t.direction=this.direction,p)p[d]===this.direction[d]&&(a[d]=!0);if(a.x&&a.y&&a.angle)return t;a.x&&a.y||this.trigger("plain",t),a.x||this.trigger("plain:"+e,t),a.y||this.trigger("plain:"+o,t),a.angle||this.trigger("dir dir:"+i,t)}else this.resetDirection();return t};var _=w;function T(t,i){return this.nipples=[],this.idles=[],this.actives=[],this.ids=[],this.pressureIntervals={},this.manager=t,this.id=T.id,T.id+=1,this.defaults={zone:document.body,multitouch:!1,maxNumberOfNipples:10,mode:"dynamic",position:{top:0,left:0},catchDistance:200,size:100,threshold:.1,color:"white",fadeTime:250,dataOnly:!1,restJoystick:!0,restOpacity:.5,lockX:!1,lockY:!1,shape:"circle",dynamicPage:!1,follow:!1},this.config(i),"static"!==this.options.mode&&"semi"!==this.options.mode||(this.options.multitouch=!1),this.options.multitouch||(this.options.maxNumberOfNipples=1),this.updateBox(),this.prepareNipples(),this.bindings(),this.begin(),this.nipples}T.prototype=new e,(T.constructor=T).id=0,T.prototype.prepareNipples=function(){var o=this.nipples;o.on=this.on.bind(this),o.off=this.off.bind(this),o.options=this.options,o.destroy=this.destroy.bind(this),o.ids=this.ids,o.id=this.id,o.processOnMove=this.processOnMove.bind(this),o.processOnEnd=this.processOnEnd.bind(this),o.get=function(t){if(void 0===t)return o[0];for(var i=0,e=o.length;i<e;i+=1)if(o[i].identifier===t)return o[i];return!1}},T.prototype.bindings=function(){this.bindEvt(this.options.zone,"start"),this.options.zone.style.touchAction="none",this.options.zone.style.msTouchAction="none"},T.prototype.begin=function(){var t=this.options;"static"===t.mode&&((t=this.createNipple(t.position,this.manager.getIdentifier())).add(),this.idles.push(t))},T.prototype.createNipple=function(t,i){var e,o,n=this.manager.scroll,s={},r=this.options;t.x&&t.y?s={x:t.x-(n.x+this.box.left),y:t.y-(n.y+this.box.top)}:(t.top||t.right||t.bottom||t.left)&&((e=document.createElement("DIV")).style.display="hidden",e.style.top=t.top,e.style.right=t.right,e.style.bottom=t.bottom,e.style.left=t.left,e.style.position="absolute",r.zone.appendChild(e),o=e.getBoundingClientRect(),r.zone.removeChild(e),s=t,t={x:o.left+n.x,y:o.top+n.y});t=new _(this,{color:r.color,size:r.size,threshold:r.threshold,fadeTime:r.fadeTime,dataOnly:r.dataOnly,restJoystick:r.restJoystick,restOpacity:r.restOpacity,mode:r.mode,identifier:i,position:t,zone:r.zone,frontPosition:{x:0,y:0},shape:r.shape});return r.dataOnly||(x(t.ui.el,s),x(t.ui.front,t.frontPosition)),this.nipples.push(t),this.trigger("added "+t.identifier+":added",t),this.manager.trigger("added "+t.identifier+":added",t),this.bindNipple(t),t},T.prototype.updateBox=function(){this.box=this.options.zone.getBoundingClientRect()},T.prototype.bindNipple=function(t){function i(t,i){e=t.type+" "+i.id+":"+t.type,o.trigger(e,i)}var e,o=this;t.on("destroyed",o.onDestroyed.bind(o)),t.on("shown hidden rested dir plain",i),t.on("dir:up dir:right dir:down dir:left",i),t.on("plain:up plain:right plain:down plain:left",i)},T.prototype.pressureFn=function(i,e,t){var o=this,n=0;clearInterval(o.pressureIntervals[t]),o.pressureIntervals[t]=setInterval(function(){var t=i.force||i.pressure||i.webkitForce||0;t!==n&&(e.trigger("pressure",t),o.trigger("pressure "+e.identifier+":pressure",t),n=t)}.bind(o),100)},T.prototype.onstart=function(e){var o=this,i=o.options,n=e;return e=r(e),o.updateBox(),c(e,function(t){o.actives.length<i.maxNumberOfNipples?o.processOnStart(t):n.type.match(/^touch/)&&(Object.keys(o.manager.ids).forEach(function(i){var t;Object.values(n.touches).findIndex(function(t){return t.identifier===i})<0&&((t=[e[0]]).identifier=i,o.processOnEnd(t))}),o.actives.length<i.maxNumberOfNipples&&o.processOnStart(t))}),o.manager.bindDocument(),!1},T.prototype.processOnStart=function(i){var e=this,t=e.options,o=e.manager.getIdentifier(i),n=i.force||i.pressure||i.webkitForce||0,s={x:i.pageX,y:i.pageY},r=e.getOrCreate(o,s);r.identifier!==o&&e.manager.removeIdentifier(r.identifier),r.identifier=o;function d(t){t.trigger("start",t),e.trigger("start "+t.id+":start",t),t.show(),0<n&&e.pressureFn(i,t,t.identifier),e.processOnMove(i)}if(0<=(o=e.idles.indexOf(r))&&e.idles.splice(o,1),e.actives.push(r),e.ids.push(r.identifier),"semi"!==t.mode)d(r);else{if(!(v(s,r.position)<=t.catchDistance))return r.destroy(),void e.processOnStart(i);d(r)}return r},T.prototype.getOrCreate=function(t,i){var e,o=this.options;return/(semi|static)/.test(o.mode)?(e=this.idles[0])?(this.idles.splice(0,1),e):"semi"===o.mode?this.createNipple(i,t):(console.warn("Coudln't find the needed nipple."),!1):e=this.createNipple(i,t)},T.prototype.processOnMove=function(t){var i=this.options,e=this.manager.getIdentifier(t),o=this.nipples.get(e),n=this.manager.scroll;if(u=t,isNaN(u.buttons)?0!==u.pressure:0!==u.buttons){if(!o)return console.error("Found zombie joystick with ID "+e),void this.manager.removeIdentifier(e);i.dynamicPage&&(f=o.el.getBoundingClientRect(),o.position={x:n.x+f.left,y:n.y+f.top}),o.identifier=e;var s=o.options.size/2,r={x:t.pageX,y:t.pageY};i.lockX&&(r.y=o.position.y),i.lockY&&(r.x=o.position.x);var d,p,a,c,l,h=v(r,o.position),u=(p=(y=o.position).x-r.x,d=y.y-r.y,Math.atan2(d,p)*(180/Math.PI)),f=b(u),e=h/s,y={distance:h,position:r};"circle"===o.options.shape?(g=Math.min(h,s),d=o.position,p=g,a={x:0,y:0},m=b(m=u),a.x=d.x-p*Math.cos(m),a.y=d.y-p*Math.sin(m),m=a):(a=r,c=o.position,l=s,m={x:Math.min(Math.max(a.x,c.x-l),c.x+l),y:Math.min(Math.max(a.y,c.y-l),c.y+l)},g=v(m,o.position)),i.follow?s<h&&(c=r.x-m.x,l=r.y-m.y,o.position.x+=c,o.position.y+=l,o.el.style.top=o.position.y-(this.box.top+n.y)+"px",o.el.style.left=o.position.x-(this.box.left+n.x)+"px",h=v(r,o.position)):(r=m,h=g);var m=r.x-o.position.x,g=r.y-o.position.y;o.frontPosition={x:m,y:g},i.dataOnly||x(o.ui.front,o.frontPosition);i={identifier:o.identifier,position:r,force:e,pressure:t.force||t.pressure||t.webkitForce||0,distance:h,angle:{radian:f,degree:u},vector:{x:m/s,y:-g/s},raw:y,instance:o,lockX:i.lockX,lockY:i.lockY};(i=o.computeDirection(i)).angle={radian:b(180-u),degree:180-u},o.trigger("move",i),this.trigger("move "+o.id+":move",i)}else this.processOnEnd(t)},T.prototype.processOnEnd=function(t){var i=this,e=i.options,t=i.manager.getIdentifier(t),o=i.nipples.get(t),t=i.manager.removeIdentifier(o.identifier);o&&(e.dataOnly||o.hide(function(){"dynamic"===e.mode&&(o.trigger("removed",o),i.trigger("removed "+o.id+":removed",o),i.manager.trigger("removed "+o.id+":removed",o),o.destroy())}),clearInterval(i.pressureIntervals[o.identifier]),o.resetDirection(),o.trigger("end",o),i.trigger("end "+o.id+":end",o),0<=i.ids.indexOf(o.identifier)&&i.ids.splice(i.ids.indexOf(o.identifier),1),0<=i.actives.indexOf(o)&&i.actives.splice(i.actives.indexOf(o),1),/(semi|static)/.test(e.mode)?i.idles.push(o):0<=i.nipples.indexOf(o)&&i.nipples.splice(i.nipples.indexOf(o),1),i.manager.unbindDocument(),/(semi|static)/.test(e.mode)&&(i.manager.ids[t.id]=t.identifier))},T.prototype.onDestroyed=function(t,i){0<=this.nipples.indexOf(i)&&this.nipples.splice(this.nipples.indexOf(i),1),0<=this.actives.indexOf(i)&&this.actives.splice(this.actives.indexOf(i),1),0<=this.idles.indexOf(i)&&this.idles.splice(this.idles.indexOf(i),1),0<=this.ids.indexOf(i.identifier)&&this.ids.splice(this.ids.indexOf(i.identifier),1),this.manager.removeIdentifier(i.identifier),this.manager.unbindDocument()},T.prototype.destroy=function(){for(var t in this.unbindEvt(this.options.zone,"start"),this.nipples.forEach(function(t){t.destroy()}),this.pressureIntervals)this.pressureIntervals.hasOwnProperty(t)&&clearInterval(this.pressureIntervals[t]);this.trigger("destroyed",this.nipples),this.manager.unbindDocument(),this.off()};var k=T;function P(t){var e=this;e.ids={},e.index=0,e.collections=[],e.scroll=d(),e.config(t),e.prepareCollections();function i(){var i;e.collections.forEach(function(t){t.forEach(function(t){i=t.el.getBoundingClientRect(),t.position={x:e.scroll.x+i.left,y:e.scroll.y+i.top}})})}s(window,"resize",function(){n(i)});function o(){e.scroll=d()}return s(window,"scroll",function(){n(o)}),e.collections}P.prototype=new e,(P.constructor=P).prototype.prepareCollections=function(){var t=this;t.collections.create=t.create.bind(t),t.collections.on=t.on.bind(t),t.collections.off=t.off.bind(t),t.collections.destroy=t.destroy.bind(t),t.collections.get=function(i){var e;return t.collections.every(function(t){return!(e=t.get(i))}),e}},P.prototype.create=function(t){return this.createCollection(t)},P.prototype.createCollection=function(t){t=new k(this,t);return this.bindCollection(t),this.collections.push(t),t},P.prototype.bindCollection=function(t){function i(t,i){e=t.type+" "+i.id+":"+t.type,o.trigger(e,i)}var e,o=this;t.on("destroyed",o.onDestroyed.bind(o)),t.on("shown hidden rested dir plain",i),t.on("dir:up dir:right dir:down dir:left",i),t.on("plain:up plain:right plain:down plain:left",i)},P.prototype.bindDocument=function(){this.binded||(this.bindEvt(document,"move").bindEvt(document,"end"),this.binded=!0)},P.prototype.unbindDocument=function(t){Object.keys(this.ids).length&&!0!==t||(this.unbindEvt(document,"move").unbindEvt(document,"end"),this.binded=!1)},P.prototype.getIdentifier=function(t){var i;return t?void 0===(i=void 0===t.identifier?t.pointerId:t.identifier)&&(i=this.latest||0):i=this.index,void 0===this.ids[i]&&(this.ids[i]=this.index,this.index+=1),this.latest=i,this.ids[i]},P.prototype.removeIdentifier=function(t){var i,e={};for(i in this.ids)if(this.ids[i]===t){e.id=i,e.identifier=this.ids[i],delete this.ids[i];break}return e},P.prototype.onmove=function(t){return this.onAny("move",t),!1},P.prototype.onend=function(t){return this.onAny("end",t),!1},P.prototype.oncancel=function(t){return this.onAny("end",t),!1},P.prototype.onAny=function(t,i){var e,o=this,n="processOn"+t.charAt(0).toUpperCase()+t.slice(1);return i=r(i),c(i,function(t){e=o.getIdentifier(t),c(o.collections,function(t,i,e){0<=e.ids.indexOf(i)&&(e[n](t),t._found_=!0)}.bind(null,t,e)),t._found_||o.removeIdentifier(e)}),!1},P.prototype.destroy=function(){this.unbindDocument(!0),this.ids={},this.index=0,this.collections.forEach(function(t){t.destroy()}),this.off()},P.prototype.onDestroyed=function(t,i){if(this.collections.indexOf(i)<0)return!1;this.collections.splice(this.collections.indexOf(i),1)};var E=new P;i.default={create:function(t){return E.create(t)},factory:E}}],n.c=o,n.d=function(t,i,e){n.o(t,i)||Object.defineProperty(t,i,{enumerable:!0,get:e})},n.r=function(t){"undefined"!=typeof Symbol&&Symbol.toStringTag&&Object.defineProperty(t,Symbol.toStringTag,{value:"Module"}),Object.defineProperty(t,"__esModule",{value:!0})},n.t=function(i,t){if(1&t&&(i=n(i)),8&t)return i;if(4&t&&"object"==typeof i&&i&&i.__esModule)return i;var e=Object.create(null);if(n.r(e),Object.defineProperty(e,"default",{enumerable:!0,value:i}),2&t&&"string"!=typeof i)for(var o in i)n.d(e,o,function(t){return i[t]}.bind(null,o));return e},n.n=function(t){var i=t&&t.__esModule?function(){return t.default}:function(){return t};return n.d(i,"a",i),i},n.o=function(t,i){return Object.prototype.hasOwnProperty.call(t,i)},n.p="",n(n.s=0).default;function n(t){if(o[t])return o[t].exports;var i=o[t]={i:t,l:!1,exports:{}};return e[t].call(i.exports,i,i.exports,n),i.l=!0,i.exports}var e,o});
)rawliteral";


void setup(){
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255,255,255,0));
  
  dnsServer.start(53, "*", apIP); //captive portal fake DNS
  
  server.on("/", []() {
    server.send_P(200, "text/html", pagina_html);
  });

  server.on("/style.css", []() {
    server.send_P(200, "text/css", style_css);
  });

  server.on("/integracao.js", []() {
    server.send_P(200, "text/javascript", integracao_js);
  });

  server.on("/soltarCarga", []() {
    Serial.println("Comando soltar carga recebido");
    server.send(200, "text/plain", "Comando soltar carga recebido");
  });

  server.on("/casa", []() {
    Serial.println("Comando voltar casa recebido");
    server.send(200, "text/plain", "Comando voltar casa recebido");
  });

  server.on("/pousar", []() {
    Serial.println("Comando pousar recebido");
    server.send(200, "text/plain", "Comando pousar recebido");
  });

  server.on("/ligarDesligar", []() {
    Serial.println("Comando ligar/desligar recebido");
    server.send(200, "text/plain", "Comando ligar recebido");
  });

  server.on("/decolar", []() {
    Serial.println("Comando decolar recebido");
    server.send(200, "text/plain", "Comando decolar recebido");
  });

  server.on("/joystickAlturaRotacao", HTTP_POST, []() {
    String body = server.arg("plain"); //pega o conteudo enviado pelo fetch
    Serial.print("Comando recebido");
    Serial.println(body);
    server.send(200, "text/plain", "Comando recebido com sucesso");
  });

  server.on("/joystickDirecoes", HTTP_POST, []() {
    String body = server.arg("plain"); //pega o conteudo enviado pelo fetch
    Serial.print("Comando recebido");
    Serial.println(body);
    server.send(200, "text/plain", "Comando recebido com sucesso");
  });

  server.on("/nipplejs.min.js", [](){
    server.send_P(200, "text/javascript", nipple_js);
  });


  server.begin();
  Serial.println("Controle Criado na rede WIFI");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}