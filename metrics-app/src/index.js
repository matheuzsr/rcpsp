import { readFile } from "fs/promises";

async function lerArquivoInstancia(caminhoArquivo) {
  try {
    const conteudo = await readFile(caminhoArquivo, "utf8");
    return conteudo;
  } catch (erro) {
    console.error("Erro ao ler o arquivo:", erro);
  }
}

const execucao = "exec_1";
const alfa = "alfa_0.850000";
const instancia = "j301_2";

// j301_2-100.000000-0.999000-300
const tempInicials = [
  "1.000000",
  // "10.000000",
  // "50.000000",
  // "100.000000",
  // "200.000000",
];
const tempFinal = "0.010000";
const taxaResfs = ["0.100000", "0.900000", "0.975000", "0.995000", "0.999000"];
const numSolVizs = ["10", "100", "300", "500"];

const getMelhorFO = (file) => {
  const regex = /FO:\s*(\d+)/g;

  const matches = file.match(regex);
  const FOs = matches.map((match) => match.match(/\d+/)[0]);

  melhoresFOs = FOs.sort();
  return melhoresFOs[0];
};

const getMediasTemperaturasIniciais = async (instancia) => {
  const mediasTemperaturas = [];

  for (temperaturaInicial of tempInicials) {
    let mediaTemperaturas = [];
    for (taxaResfriamento of taxaResfs) {
      for (numSolVizinho of numSolVizs) {
        const file = await lerArquivoInstancia(
          `../metricas/${execucao}/j90-${temperaturaInicial}-${taxaResfriamento}.metric`
        );
        mediaTemperaturas.push(Number(getMelhorFO(file)));
      }
    }

    const media = calcularMedia(mediaTemperaturas);

    mediasTemperaturas.push({
      text: temperaturaInicial,
      media,
    });
  }

  return mediasTemperaturas;
};


const getMediasNumSolVizs = async (instancia) => {
  const mediasTaxaRefs = [];
  const temperaturaInicial = "1.000000";

  for (taxaResfriamento of taxaResfs) {
    const file = await lerArquivoInstancia(
      `../metricas/${execucao}/j90-${temperaturaInicial}-${taxaResfriamento}.metric`
      );

    mediasTaxaRefs.push({
      text: taxaResfriamento,
      value: Number(getMelhorFO(file)),
    });
  }

  return mediasTaxaRefs;
};

const main = async () => {
  // console.log("MediasTemperaturasIniciais");
  // console.log(await getMediasTemperaturasIniciais(instancia));

  console.log("Taxa resfriamento");
  console.log(await getMediasNumSolVizs(instancia));
};

export const calcularMedia = (vetor) => {
  return vetor.reduce((acc, num) => acc + num, 0) / vetor.length;
};

main();
