const { exec, spawn } = require("child_process");
const fs = require('fs')


link_base = `https://cdn.gea.esac.esa.int/Gaia/gedr3/simulation/gaia_universe_model/`
filename = `GaiaUniverseModel_#.csv.gz`

md5_sums = fs.readFileSync('_MD5SUM.txt', { encoding: 'utf8', flag: 'r' });

cnt = 0
active = {}

async function getFile(file) {
    cnt ++
    var link = link_base + file
    console.log(`[${Date.now()}]: ${link}. Cnt: ${cnt}`)
    exec(`wget ${link}`, { cwd: '/tmp' }, function (e, out, stderr) {
        if (e) {
            console.log(e)
            cnt--
            return e;
        }
        exec(`md5sum ${file}`, { cwd: '/tmp' }, function (e, a, out) {
            if (e) {
                console.log(e)
                cnt--
                return e;
            }
            a.split('\n').join('')
            if (md5_sums.includes(a)) {
                console.log(`File ${file} downloaded !`)
                parse_file(file)
            }
            else {
                exec(`rm ${file}*`, { cwd: '/tmp' }, function (e, out, stderr) {})
                console.log(`For file ${file}, MD5sum is incorrect!`)
                cnt--
                return
            }
        });
    });
}

async function parse_file(file) {
    exec(`gzip -d ${file}`, { cwd: '/tmp' }, function (e, out, stderr) {
        if (e) {
            console.log(e)
            cnt--
            return e;
        }
        var new_fn = file.split('.gz')[0]
        var new_fnraw = file.split('.csv.gz')[0] + '.raw'
        exec(`bash rf.sh ${new_fn} ${new_fnraw}`, function (e, out, stderr) {
            if (e) {
                console.log(e)
                cnt--
                return e;
            }
            console.log(`${new_fnraw} finished`)
            cnt--
            /* exec(`rm ${new_fn}*`, { cwd: '/tmp' }, function (e, out, stderr) {
                if (e) {
                    cnt--
                    return e;
                }
                console.log(out)
            }) */
        })
    })
}
async function loop(i) {
    for (i; i < 5000; i++) {
        ii = i + ''
        while (ii.length < 4)
            ii = '0' + ii
        fn = filename.split('#').join(ii)
        cck = fn.split('.csv.gz')[0] + '.raw'
        if(cnt >= 8){
            setTimeout(loop, 1000, i);
            return
        }
        if (!fs.existsSync(`/media/adespawn/01524459-1aff-4407-a4a6-68c6a130a898/data/hackaton/${cck}`)) {
            if(!fs.existsSync('.lock')){
                console.log(`No lock. Exiting!!!!`)
                return
            }
            getFile(fn)
            // console.log(fn)
        }
    }

}

loop(0)
// for (i = 0; i < 5000; i++) {
