window.onload = function() {
    console.log('start dracula');
    var g = new Graph();
    
    g.addEdge("strawberry", "cherry");
    g.addEdge("strawberry", "apple");
    g.addEdge("strawberry", "tomato");
    
    g.addEdge("tomato", "apple");
    g.addEdge("tomato", "kiwi");
    
    g.addEdge("cherry", "apple", { stroke: "#bfa", fill: "#abc", label: "1" });
    g.addEdge("cherry", "kiwi");
    
    var layouter = new Graph.Layout.Spring(g);
    layouter.layout();
    
    var renderer = new Graph.Renderer.Raphael('canvas', g, 400, 300);
    renderer.draw();
    console.log('stop dracula');
}
