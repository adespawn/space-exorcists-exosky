const path = require('path');
const execSync = require('child_process').execSync;
const { exec, spawn } = require("child_process");
const fs = require('fs')


x = parseInt(process.argv[2])
y = parseInt(process.argv[3])
z = parseInt(process.argv[4])

base_dir = `/media/adespawn/01524459-1aff-4407-a4a6-68c6a130a898/data/layer_0`

tmp_file = `/tmp/${process.pid}`
ans_file = `/tmp/${process.pid}.ans`

function add_file(file) {
    code = execSync(`cat ${file} >>${tmp_file}`);
}

function in_range(info_file) {
    data = fs.readFileSync(info_file, { encoding: 'utf8', flag: 'r' })
    x_mn = parseInt(data.split(' ')[0])
    x_mx = parseInt(data.split(' ')[1])
    y_mn = parseInt(data.split(' ')[2])
    y_mx = parseInt(data.split(' ')[3])
    z_mn = parseInt(data.split(' ')[4])
    z_mx = parseInt(data.split(' ')[5])

    /* console.log(`${x}: ${x_mn} == ${x_mx}`)
    console.log(`${y}: ${y_mn} == ${y_mx}`)
    console.log(`${z}: ${z_mn} == ${z_mx}`) */

    if (!(x_mn <= x && x <= x_mx))
        return false;

    if (!(y_mn <= y && y <= y_mx))
        return false;

    if (!(z_mn <= z && z <= z_mx))
        return false;

    // console.log(`=================TRUE=================`)
    return true;
}

function recursive_search(dir) {
    console.log(`Doing recursive search in ${dir}`)
    let files = fs.readdirSync(dir);
    for (let i = 0; i < files.length; i++) {
        let file = files[i]
        if (!file.includes('.info'))
            continue;
        let base_name = file.split('.info')[0]
        if (!fs.existsSync(path.join(dir, `${base_name}.top`)))
            continue;
        if (in_range(path.join(dir, `${base_name}.info`))) {
            // console.log(`IN RANGE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!`)
            if (!fs.existsSync(path.join(dir, `${base_name}.raw`))) {
                recursive_search(path.join(dir, base_name))
            } else {
                // console.log(`Getting raw file: ${dir}::${base_name}.raw `)
                add_file(path.join(dir, `${base_name}.raw`))
            }
        }
        else {
            // console.log(`Getting top file: ${dir}::${base_name}.top`)
            add_file(path.join(dir, `${base_name}.top`))
        }

    }
    console.log(`EXITING RANGE: ${dir}`)
}


exec(`touch ${tmp_file}`, function (e, out, stderr) {
    if (e) {
        console.log(e)
        return
    }
    console.log(`Created tmp file: ${tmp_file}`)
    recursive_search(base_dir)
    execSync(`bash ./api.sh ${tmp_file} ${ans_file}`)
})
