const rbt_api = require('./rbt_api.js');
const fs = require('fs');
const wasm = fs.readFileSync("RedBlackTree.wasm");
WebAssembly.instantiate(wasm, { wasi_unstable: {
          printString: function(){},
          clearScreen: function(){},
          noMoreFreeMemory: function(){},
          segmentationFault: function(){},
          getLengthOfTheInput: function(){},
          getCharacterOfTheInput: function(){},
          drawRectangle: function(){},
          drawLine: function(){},
          drawText: function(){},
          setDiagramWidth: function(){},
          setDiagramHeight: function(){},
        },
      }).then((module)=>{
	rbt_api.RBTAPI.init(module).then((rbtapi)=>{
	const preorder = rbtapi.setKeysRenderAndGetPreorder([1, 2, 3]);
	if (preorder == "2 1 3 ") {
		console.log("The test has passed!");
		process.exitCode = 0;
	}
	else {
		console.log("The test has failed!");
		console.log(preorder);
		process.exitCode = 1;
	}
	});
});
