// Referências dos elementos
var joystickAlturaRotacao = document.getElementById("joystickAlturaRotacao");
var joystickDirecoes = document.getElementById("joystickDirecoes");
var botaoLigarDesligar = document.getElementById("BotaoLigarDesligar");
var botaoVoltarCasa = document.getElementById("BotaoVoltarCasa");
var botaoSoltarCarga = document.getElementById("BotaoSoltarCarga");
var botaoDecolar = document.getElementById("BotaoDecolar");
var botaoPousar = document.getElementById("BotaoPousar");

// Controle de envio de dados
let podeEnviarAlturaRotacao = true;
let podeEnviarDirecoes = true;

// Estados dos joysticks
var EstadoJoystickAlturaRotacao = { Direcao: "", Forca: 0 };
var EstadoJoystickDirecoes = { Direcao: "", Forca: 0 };

// Configuração do joystick de altura e rotação
joystickAlturaRotacao = nipplejs.create({
  zone: joystickAlturaRotacao,
  mode: 'static',
  position: { left: '250px', top: '50%' },
  color: 'gray',
  size: 160
});

joystickAlturaRotacao.on('move', function(event, data) {
  if (podeEnviarAlturaRotacao) {
    EstadoJoystickAlturaRotacao.Direcao = data.angle?.degree;
    EstadoJoystickAlturaRotacao.Forca   = data.distance;

    fetch('/joystickAlturaRotacao', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ EstadoJoystickAlturaRotacao })
    })
    .then(response => response.text())
    .then(respostaDoServidor => {
      console.log('Resposta do servidor: ', respostaDoServidor);
    })
    .catch(error => {
      console.error('Erro: ', error);
    });

    podeEnviarAlturaRotacao = false;
    setTimeout(() => {
      podeEnviarAlturaRotacao = true;
    }, 100);
  }
});

// Configuração do joystick de direções
joystickDirecoes = nipplejs.create({
  zone: joystickDirecoes,
  mode: 'static',
  position: { right: '250px', top: '50%' },
  color: 'gray',
  size: 160
});

joystickDirecoes.on('move', function(event, data) {
  if (podeEnviarDirecoes) {
    EstadoJoystickDirecoes.Direcao = data.angle?.degree;
    EstadoJoystickDirecoes.Forca   = data.distance;

    fetch('/joystickDirecoes', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ EstadoJoystickDirecoes })
    })
    .then(response => response.text())
    .then(respostaDoServidor => {
      console.log('Resposta do servidor: ', respostaDoServidor);
    })
    .catch(error => {
      console.error('Erro: ', error);
    });

    podeEnviarDirecoes = false;
    setTimeout(() => {
      podeEnviarDirecoes = true;
    }, 100);
  }
});

// Endpoints para funções do drone
botaoLigarDesligar.addEventListener("click", function() {
  fetch("/ligarDesligar").then(response => {
    if (response.ok) {
      console.log("Comando ligar-desligar enviado");
    }
  });
});

botaoDecolar.addEventListener("click", function() {
  fetch("/decolar").then(response => {
    if (response.ok) {
      console.log("Comando decolar enviado");
    }
  });
});

botaoPousar.addEventListener("click", function() {
  fetch("/pousar").then(response => {
    if (response.ok) {
      console.log("Comando pousar enviado");
    }
  });
});

botaoVoltarCasa.addEventListener("click", function() {
  fetch("/casa").then(response => {
    if (response.ok) {
      console.log("Comando voltar para casa enviado");
    }
  });
});

botaoSoltarCarga.addEventListener("click", function() {
  fetch("/soltarCarga").then(response => {
    if (response.ok) {
      console.log("Comando soltar carga enviado");
    }
  });
});