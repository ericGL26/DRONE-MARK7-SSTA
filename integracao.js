var joystickAlturaRotacao = document.getElementById("joystickAlturaRotacao")
var joystickDirecoes = document.getElementById("joystickDirecoes")
var botaoLigarDesligar = document.getElementById("BotaoLigarDesligar")
var botaoVoltarCasa = document.getElementById("BotaoVoltarCasa")
var botaoSoltarCarga = document.getElementById("BotaoSoltarCarga")
var botaoDecolar = document.getElementById("BotaoDecolar")
var botaoPousar = document.getElementById("BotaoPousar")

var EstadoJoystickAlturaRotacao = {Direcao: "", Forca: 0}
var EstadoJoystickDirecoes = {Direcao: "", Forca: 0}

//configuracao do joystick altura e rotacao
joystickAlturaRotacao = nipplejs.create({
    zone : joystickAlturaRotacao,
    mode: 'static',
    position: {left: '215px', top: '50%'},
    color: 'gray',
    size: 300
})

joystickAlturaRotacao.on('move', function(event, data) {
    EstadoJoystickAlturaRotacao.Direcao = data.angle?.degree;
    EstadoJoystickAlturaRotacao.Forca = data.distance;

    //envia os dados coletados pelo joystick para a controladora(ESP8266)
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
    })
});

//configuracao do joystick direcoes
joystickDirecoes = nipplejs.create({
    zone: joystickDirecoes,
    mode: 'static',
    position: {left: '750px', top: '50%'},
    color: 'gray',
    size: 300
})

joystickDirecoes.on('move', function(event, data) {
    EstadoJoystickDirecoes.Direcao = data.angle?.degree;
    EstadoJoystickDirecoes.Forca = data.distance;

    //envia os dados coletados pelo joystick para a controladora(ESP8266)
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
    })
})


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

