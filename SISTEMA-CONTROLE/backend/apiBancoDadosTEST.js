const axios = require("axios")
const assert = require("assert")
const datahora = new Date();

const datahoraFormatada = datahora.toLocaleString("pt-BR", {
  timeZone: "America/Sao_Paulo"
});

describe("Suite de testes da api do banco de dados", () => {
  describe("Suite de testes da rota /API/PostDados", () => {
    it("Teste enviar dados corretos", async () => {
      const dados = {
        rotaFeita: "Rever",
        horaDecolagem: datahoraFormatada,
        horaPouso: datahoraFormatada,
        usuario: "ERIC",
        dispositivoControlador: "XIAOMI REDMI NODE 14"
      };

      try {
        const resposta = await axios.post("http://localhost:3301/API/PostDados", dados)
        assert.equal(resposta.data.message, "Dados inseridos com sucesso")
        console.log("Resposta: ", resposta.data.message)
      } catch(error) {
        console.error("Error: ", error)
        throw error; // Isso fará o teste falhar caso haja uma exceção.
      }
    })

  })
})