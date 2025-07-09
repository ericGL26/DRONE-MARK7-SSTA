//CONEXAO COM O BANCO DE DADOS
const { MongoClient, ServerApiVersion } = require('mongodb');
const uri = "mongodb+srv://ericgomes26012010:OTEkMUqkLV3j3lkN@banco-drone.sxcp8uc.mongodb.net/?retryWrites=true&w=majority&appName=BANCO-DRONE";
const express = require("express")
const bodyParser= require("body-parser")

const app = express() //app representa o servidor web
const port = 3301;

app.use(bodyParser.json()) // permite ler JSON no corpo das requisições
app.use(express.json())

const client = new MongoClient(uri, {
  serverApi: {
    version: ServerApiVersion.v1,
    strict: true,
    deprecationErrors: true,
  }
});

async function run() {
  try {
    await client.connect() //conecta o cliente no servidor
    await client.db("admin").command({ ping: 1 }) //ping 1 basicamente pergunta para o banco se ele está online e ouvindo
    const db = client.db("BANCO-DRONE")
    const colecaoBanco = db.collection("RegistrosDeVoo");
    console.log("conexao bem sucedida")

    //Rota POST para inserir dados
    app.post("/API/PostDados", async(requisicao, resposta) => {
      try{
        const dadosRecebidos = requisicao.body
        const InserirDados = await colecaoBanco.insertOne(dadosRecebidos);

        resposta.status(201).json({
          message: "Dados inseridos com sucesso",
          id: InserirDados.insertId,
        });
      } catch(error){
        resposta.status(500).json({message: "Erro ao inserir dados", error})
      }
    })

    //inicia o servidor na porta definida
    app.listen(port, () => {
      console.log("API rodando na porta 3301")
    })

  } catch(error) {
    console.log("Error ao iniciar a API: ", error)
  }
}
run().catch(console.dir)