import { readFile } from "fs/promises";

async function lerArquivoMetricas(caminhoArquivo) {
  try {
    const conteudo = await readFile(caminhoArquivo, "utf8");
    return conteudo;
  } catch (erro) {
    console.error("Erro ao ler o arquivo:", erro);
  }
}

const execs = ["exec_1"];

const instancias = [
  "j30/j301_1",
  "j30/j301_2",
  "j30/j301_3",
  "j30/j301_4",
  "j30/j301_5",

  "j60/j601_1",
  "j60/j601_2",
  "j60/j601_3",
  "j60/j601_4",
  "j60/j601_5",

  "j90/j901_1",
  "j90/j901_2",
  "j90/j901_3",
  "j90/j901_4",
  "j90/j901_5",

  "j120/j1201_1",
  "j120/j1201_2",
  "j120/j1201_3",
  "j120/j1201_4",
  "j120/j1201_5",
];

const getMelhorFO = (file) => {
  const regex = /FO:\s*(\d+)/g;

  return file.match(regex)[0].match(/\d+/)[0];
};

const main = async () => {
  let i = 0;
  for (instancia of instancias) {
    if((i % 5) === 0) {
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
