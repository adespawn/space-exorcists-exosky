// args: 
//      - current layer folders list
//      - place to write next layer
const fs = require('fs')
const path = require('path');
const { exec, spawn } = require("child_process");

function work_folder(c_path) {
    if (c_path.length < 2)
        return
    console.log(`Merging data from folders: ${c_path}`)
    let _d = c_path.substring(0, c_path.length - 1)
    let cmd = `bash ./merge_files.sh ${c_path} ${_d}.top &>>../.log/phase_3_cpp`
    exec(cmd, function (e, out, stderr) { })
}

for (let i = 2; i < process.argv.length; i++) {
    folders = fs.readFileSync(process.argv[i], { encoding: 'utf8', flag: 'r' })
    folders = folders.split('\n')
    for (let i = 0; i < folders.length; i++) {
        work_folder(folders[i])
    }
}
