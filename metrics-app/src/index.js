import { readFile } from "fs/promises";

async function lerArquivoInstancia(caminhoArquivo) {
  try {
    const conteudo = await readFile(caminhoArquivo, "utf8");
    return conteudo;
  } catch (erro) {
    console.error("Erro ao ler o arquivo:", erro);
  }
}

const execs = ["exec_1", "exec_2", "exec_3", "exec_4", "exec_5"];
const alfas = [
  "alfa_0.000000",
  "alfa_0.200000",
  "alfa_0.500000",
  "alfa_0.800000",
  "alfa_1.000000",
];
const instancias = [
  "j301_1",
  "j301_2",
  "j609_3",
  "j609_8",
  "j1206_4",
  "j1207_2",
];

const getMelhorFO = (file) => {
  const regex = /FO:\s*(\d+)/g;

  const matches = file.match(regex);
  const FOs = matches.map((match) => match.match(/\d+/)[0]);

  melhoresFOs = FOs.sort();
  return melhoresFOs[0];
};

const main = async () => {
  for (instancia of instancias) {
    console.log("\nInstancia " + instancia + ":");
    for (alfa of alfas) {
      console.log("\nAlfa " + alfa + ":\n");
      for (execucao of execs) {
        console.log;
        const file = await lerArquivoInstancia(
          `../metricas/${execucao}/${alfa}/${instancia}.metric`
        );
        console.log(execucao + ": " + getMelhorFO(file));
      }
    }
  }
};

main();
