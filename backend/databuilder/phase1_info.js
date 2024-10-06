rng = ['-200000 -8000 ', '-8000 -2000 ', '-2000 2000 ', '2000 8000 ', '8000 200000 ']

path = '/media/adespawn/01524459-1aff-4407-a4a6-68c6a130a898/data/layer_0/'
const fs = require('fs')

for(let i=0; i<5; i++){
    for(let j=0;j<5;j++){
        for(let k=0;k<5;k++){
            filename = `0_${i}_${j}_${k}.info`
            fs.writeFileSync(path+filename, rng[i]+rng[j]+rng[k])
        }
    }
}
