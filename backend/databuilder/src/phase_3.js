// args: 
//      - current layer folders list
//      - place to write next layer
const fs = require('fs')
const path = require('path');
const { exec, spawn } = require("child_process");

function work_file(c_path, c_file) {
    console.log(`Starting working for file ${c_path}${c_file}!`)
    let base_filename = c_file.split('.raw')[0];
    exec(`bash ./exe3.sh ${c_path}${c_file} ${c_path}${base_filename}.top &>>../.log/phase_3_cpp`, function (e, out, stderr) {
        if (e) {
            console.log(e)
        }
        console.log(`${c_path}${c_file} finished processing!`)
    })

}

function work_folder(c_path) {
    if (c_path.length < 2)
        return
    console.log(`Starting to work on path: ${c_path}`)

    fs.readdir(c_path, (err, files) => {
        files.forEach(file => {
            if (!file.includes('.raw'))
                return;
            work_file(c_path, file)

        });
    });
}

for (let i = 2; i < process.argc; i++) {
    folders = fs.readFileSync(process.argv[i], { encoding: 'utf8', flag: 'r' })
    folders = folders.split('\n')
    folders.forEach(element => {
        work_folder(element)
    });
}
