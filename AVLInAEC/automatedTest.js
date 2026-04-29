const avl_api = require("./avl_api.js");
const fs = require("fs");
const bytes = fs.readFileSync("AVLInAEC.wasm");
WebAssembly
    .instantiate(bytes, {
      wasi_unstable : {
        printString : function() {},
        clearScreen : function() {},
        noMoreFreeMemory : function() {},
        segmentationFault : function() {},
        getLengthOfTheInput : function() {},
        getCharacterOfTheInput : function() {},
        drawRectangle : function() {},
        drawLine : function() {},
        drawText : function() {},
        setDiagramWidth : function() {},
        setDiagramHeight : function() {},
      },
    })
    .then((instance) => {
      avl_api.AVLAPI.init(instance).then((avlapi) => {
        avlapi.pushKey(4);
        avlapi.pushKey(1);
        avlapi.pushKey(9);
        avlapi.pushKey(0);
        avlapi.pushKey(3);
        avlapi.pushKey(7);
        avlapi.pushKey(10);
        avlapi.pushKey(2);
        avlapi.pushKey(5);
        avlapi.pushKey(8);
        avlapi.pushKey(11);
        avlapi.pushKey(6);
        avlapi.deleteKey(
            0); // This should trigger four rotations (the maximal number of
                // rotations per a deletion in an AVL tree).
        avlapi.render();
        const memory = new Int8Array(avlapi.getMemory().buffer);
        let pointer = avlapi.exports.getAddressOfPreorderTraversal();
        let preorder = "";
        while (memory[pointer]) {
          preorder += String.fromCharCode(memory[pointer]);
          pointer++;
        }
        if (preorder == "7 4 2 1 3 5 6 9 8 10 11 ") {
          process.exitCode = 0;
          console.log("The test succeeded!");
        } else {
          process.exitCode = 1;
          console.log("The test failed!");
        }
      });
    });
