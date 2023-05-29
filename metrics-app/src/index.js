import { readFile } from 'fs/promises';

async function lerArquivo(caminhoArquivo) {
  try {
    const conteudo = await readFile(caminhoArquivo, 'utf8');
    return conteudo;
  } catch (erro) {
    console.error('Erro ao ler o arquivo:', erro);
  }
}

// alfa_0.000000  alfa_0.200000  alfa_0.500000  alfa_0.800000  alfa_1.000000

// "j301_1",
//       "j301_2",
//       "j609_3",
//       "j609_8",
//       "j1206_4",
//       "j1207_2",


const main = async () => {

  const arq = await lerArquivo('../metricas/alfa_1.000000/j1207_2.metric');
  
  const regex = /FO:\s*(\d+)/g;

  const matches = arq.match(regex);
  const FOs = matches.map(match => match.match(/\d+/)[0]);

  melhoresFOs = FOs.sort()

  console.log(numeros)
}

main()