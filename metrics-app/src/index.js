import { readFile } from "fs/promises";

async function lerArquivoMetricas(caminhoArquivo) {
  try {
    const conteudo = await readFile(caminhoArquivo, "utf8");
    return conteudo;
  } catch (erro) {
    console.error("Erro ao ler o arquivo:", erro);
  }
}

const execs = ["exec_5"];

const instancias = [
  "j30/j301_1",
  "j30/j301_2",
  "j30/j3024_9",
  "j30/j3024_10",
  "j30/j3048_9",
  "j30/j3048_10",

  "j60/j601_1",
  "j60/j601_2",
  "j60/j6024_9",
  "j60/j6024_10",
  "j60/j6048_9",
  "j60/j6048_10",

  "j90/j901_1",
  "j90/j901_2",
  "j90/j9024_9",
  "j90/j9024_10",
  "j90/j9048_9",
  "j90/j9048_10",

  "j120/j1201_1",
  "j120/j1201_2",
  "j120/j12030_9",
  "j120/j12030_10",
  "j120/j12060_9",
  "j120/j12060_10",
];

const getMelhorFO = (file) => {
  const regex = /FO:\s*(\d+)/g;

  return file.match(regex)[0].match(/\d+/)[0];
};

const main = async () => {
  let i = 0;
  for (instancia of instancias) {
    if((i % 6) === 0) {
      console.log(instancia + ":");
    }

    for (execucao of execs) {
      const file = await lerArquivoMetricas(
        `../metricas/${execucao}/${instancia}.sol`
      );
      console.log(getMelhorFO(file));
    }

    i++;
  }
};

main();
