make wasm
cp tetrism.wasm ../DecklynKern.github.io/tetrism/tetrism.wasm
cp tetrism.js ../DecklynKern.github.io/tetrism/main.js
cp tetrism.data ../DecklynKern.github.io/tetrism/tetrism.data
cd ../DecklynKern.github.io
git add tetrism/tetrism.wasm
git add tetrism/main.js
git add tetrism/tetrism.data
git commit -m "update tetrism (c project was updated)"
git push