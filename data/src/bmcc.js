class BMCC {
  constructor() {
    this.REC = 0;
    this.WhiteBalance = 1024;
  }

  sendBMCC(data, value) {
    let obj = {
      BMCC: {
        [data]: value
      }
    };
    sendJSON(obj);
  }
}

function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

const myBMCC = new BMCC();

// const BMCCAttribute = "Iris"
// document.addEventListener('keydown', function (event) {

//   if (event.key === "ArrowLeft") {
//     myBMCC.sendBMCC(BMCCAttribute, 392)
//   } else if (event.key === "ArrowRight") {
//     myBMCC.sendBMCC(BMCCAttribute, 1692)
//   } else if (event.key === "ArrowDown") {
//     myBMCC.sendBMCC(BMCCAttribute, 1024)
//   }
// });



/**
 * IRIS = slider
 * ISO = up and down
 * REC = ??
 * Audiolever = slider
 * Framerate = slider
 * Codec = slider
 * 
 */