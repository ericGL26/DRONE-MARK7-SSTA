var joystickAlturaRotacao = document.getElementById("joystickAlturaRotacao")
var joystickDirecoes = document.getElementById("joystickDirecoes")

//configuracao do joystick altura e rotacao
joystickAlturaRotacao = nipplejs.create({
    zone : joystickAlturaRotacao,
    mode: 'static',
    position: {left: '215px', top: '50%'},
    color: 'gray',
    size: 300
})

joystickAlturaRotacao.on('move', function(event, data) {
    console.log('Direcao: ', data.angle?.degree, 'Distancia', data.distance);
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
    console.log('Direcao: ', data.angle?.degree, 'Distancia', data.distance);
})

