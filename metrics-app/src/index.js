import { readFile } from "fs/promises";

async function lerArquivoInstancia(caminhoArquivo) {
  try {
    const conteudo = await readFile(caminhoArquivo, "utf8");
    return conteudo;
  } catch (erro) {
    console.error("Erro ao ler o arquivo:", erro);
  }
}

const execs = [
  "exec_1",
  "exec_2",
  "exec_3",
  "exec_4",
  "exec_5",
  "exec_6",
  "exec_7",
  "exec_8",
  "exec_9",
  "exec_5",
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
  const regex = /Melhor FO:\s*(\d+)/g;

  return file.match(regex);
};

const main = async () => {
  for (instancia of instancias) {
    console.log("\nInstancia " + instancia + ":");
    for (execucao of execs) {
      const file = await lerArquivoInstancia(`../metricas/${execucao}/${instancia}.sol`);

      console.log(getMelhorFO(file));
    }
  }
};

main();
