// Make an instance of two and place it on the page.
var two = new Two({
    type: Two.Types.canvas,
    width: 285,
    height: 200,
    domElement: document.getElementById('draw-shapes')
});

// Two has convenience methods to create shapes.
var p1 = two.makeCircle(98, 100, 10);
var p2 = two.makeCircle(100, 101, 10);
var p3 = two.makeCircle(102, 99, 10);
let points = [p1, p2, p3];
let v1 = [0, 0];
let v2 = [0, 0];
let v3 = [0, 0];
let velocities = [v1, v2, v3];

var r = 1.0;
let pRads = [r, r, r];
let numPoints = 3;

p1.fill = '#ff8000';
p2.fill = '#ff8000';
p3.fill = '#ff8000';

// Constants
var r0 = 2.0;
var m = 5;
var n = 3;
var b1 = 1;
var Cr = r0;
var Cd = r0;
var b2 = b1 * Math.pow(r0, n-m);

let time = 1/60;





// Bind a function to scale and rotate the group to the animation loop.
two.bind('update', function(frameCount) {
    // This code is called everytime two.update() is called.
    // Effectively 60 times per second.
    let a1 = [0, 0];
    let a2 = [0, 0];
    let a3 = [0, 0];
    let accelerations = [a1, a2, a3];
    for (var p = 0; p < numPoints; p++){

        for(var i = 0; i < numPoints; i++){
            let Fip = [0, 0];
            if (i != p){
                let repulsionTerm = 0;
                let dragTerm = 0;
                let D = Math.sqrt(
                    Math.pow(points[i].translation.x - points[p].translation.x, 2) +
                    Math.pow(points[i].translation.y - points[p].translation.y, 2)
                );
                console.log("P:", p, "i:", i, "D:", D)
                let pHat = [
                    points[i].translation.x - points[p].translation.x,
                    points[i].translation.y - points[p].translation.y
                ];
                let Sr = 1 - Math.pow(D, 2)/(Math.pow(Cr, 2) * Math.pow(pRads[i] + pRads[p], 2));
                let Sd = 1 - Math.pow(D, 2)/(Math.pow(Cd, 2) * Math.pow(pRads[i] + pRads[p], 2));
                console.log("Sr:", Sr);
                console.log("Sd:", Sd);
                if (Sr < 0.01){
                    Sr = 0;
                } else {
                    repulsionTerm = Sr * (b1/Math.pow(D, m) - b2/Math.pow(D, n));
                }
                console.log("Repulsionterm:", repulsionTerm);
                if (Sd < 0.01){
                    Sd = 0;
                } else {
                    let vHat = [
                        velocities[i][0] - velocities[p][0], 
                        velocities[i][0] - velocities[p][0]
                    ];
                    dragTerm = Sd * (vHat[0] * pHat[0] + vHat[1] + pHat[1])/Math.pow(D, 2);
                }
                Fip = [
                    pHat[0] * (repulsionTerm - dragTerm),
                    pHat[1] * (repulsionTerm - dragTerm)
                ];
            }
            accelerations[p] = [
                accelerations[p][0] + Fip[0],
                accelerations[p][1] + Fip[1] + 1.1
            ];
        }
    
        velocities[p] = [
            velocities[p][0] + accelerations[p][0] * time,
            velocities[p][1] + accelerations[p][1] * time
        ];
    
        points[p].translation.set(
            points[p].translation.x + velocities[p][0] * time,
            points[p].translation.y + velocities[p][1] * time
        );

        if (points[p].translation.y > 140){
            points[p].translation.set(
                points[p].translation.x,
                140
            );
        }

        if (points[p].translation.x > 150){
            points[p].translation.set(
                150,
                points[p].translation.y
            );
        }

        if (points[p].translation.x < 0){
            points[p].translation.set(
                0,
                points[p].translation.y
            );
        }
    }
    
}).play(); // Finally, start the animation loop.*/

// Don't forget to tell two to render everything to the screen
two.update();