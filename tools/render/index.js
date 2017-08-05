var colors = ["#f7eda3","#f7e771","#b8e847","#b8f957","#2fe292", "#6782d3","#516ecc","#4b8ddd","#f2d5ae","#f9a9d7", "#0b9342","#fce5c4","#87d14f","#82f2c7","#eac885", "#fffdad","#abffa8","#907ae8","#e5764e","#53d1ca", "#7951ba","#90e80d","#7fe8ae","#e0320f","#7de8d9", "#fc6cdd","#8c0e12","#dd738c","#3e69c1","#6ae8ab"]

function isclaimed(data, river) {
    var s = river.source;
    var t = river.target;
    if (data.used) {
        var key = "" + s + "," + t;
        var v = data.used[key];
        if (typeof(v) === 'number') { return "c:" + v; }
        key = "" + t + "," + s;
        v = data.used[key];
        if (typeof(v) === 'number') { return "c:" + v; }
    }
    return null;     
}

/* add a node with a customized shape 
   (the Raphael graph drawing implementation can draw this shape, please 
   consult the RaphaelJS reference for details http://raphaeljs.com/) */
function render(r, n, fillColor) {
    /* the Raphael set is obligatory, containing all you want to display */
    var set = r.set().push(
        /* custom objects go here */
        r.rect(n.point[0]-30, n.point[1]-13, 62, 66).attr({"fill": fillColor, "stroke-width": 2, r : "9px"})).push(
            r.text(n.point[0], n.point[1] + 20, n.label).attr({"font-size":"14px"}));
    return set;
};
function renderLambda(r,n) { return render(r, n, "#DE5B5B"); }
function renderNormal(r,n) { return render(r, n, "#96F07F"); }

var mines = {};

window.onload = function() {
    console.log('start dracula');
    var g = new Graph();
 
    for (var i = 0; i < data.mines.length; i++) {
        var mine = data.mines[i];
        mines[mine] = true;
    }

    for (var i = 0; i < data.sites.length; i++) {
        var site = data.sites[i];
        g.addNode(site.id, { label: mines[site.id] ? "(λ) " + site.id : site.id, render: mines[site.id] ? renderLambda : renderNormal });
    }

    for (var i = 0; i < data.rivers.length; i++) {
        var river = data.rivers[i];
        var claimed = isclaimed(data, river);
        if (claimed !== null) {
            var color = colors[claimed % colors.length];
            g.addEdge(river.source, river.target, { stroke: "#bfa", fill: color, label: claimed });
        } else {
            g.addEdge(river.source, river.target);
        }
    }
/*
    g.addEdge("strawberry", "cherry");
    g.addEdge("strawberry", "apple");
    g.addEdge("strawberry", "tomato");
    
    g.addEdge("tomato", "apple");
    g.addEdge("tomato", "kiwi");
    
    g.addEdge("cherry", "apple", { stroke: "#bfa", fill: "#abc", label: "1" });
    g.addEdge("cherry", "kiwi");
*/
    var layouter = new Graph.Layout.Spring(g);
    layouter.layout();
    
    var renderer = new Graph.Renderer.Raphael('canvas', g, 800, 600);
    renderer.draw();
    console.log('stop dracula');
}
