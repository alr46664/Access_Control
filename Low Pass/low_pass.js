"use strict";
const filterOutput = (vin,f,c,r) => {
    const xc = 1.0/(2*Math.PI*f*c)
    return vin*(xc/Math.sqrt(r*r + xc*xc));
};

let v,f,c,r;
v = process.argv[2];
f = process.argv[3];
c = process.argv[4];
r = process.argv[5];
console.log(`Vin = ${v} v  -  F = ${f} Hz  -  C = ${c} Farad  -  R = ${r} ohm \n`);
console.log(`Vout = ${filterOutput(v,f,c,r)} v`);
