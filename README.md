# Procedural sphere mesh

UV sphere mesh created procedurally<br>by indexed drawing using OpenGL graphics API.

### Generation technique

Below diagram shows how each subdivision of the sphere is constructed

````
v1  v1+1
|   /|
|  / |
| /  |
|/   |
v2  v2+1
````

The code below iterates over the stacks and sectors of the sphere and generates indices for two triangles per sector. 
The indices are stored in a vector<int> called "indices". The generated triangles form a sphere with smooth shading, based on the number of stacks and sectors supplied
More the numbers supplied, smoother the sphere. But computational time increases too

````
std::vector<int> indices;
int v1, v2;
for (int i = 0; i < Stacks; ++i) {
	v1 = i*(Stacks);
	v2 = v1 + Sectors;
	for (int j = 0; j < Sectors; j++) {
		//for bottom stack only single triangle required
		if (j != 0) {
			indices.push_back(v1);
			j != Sectors - 1 ? indices.push_back(v1 + 1) : indices.push_back(v1 - Sectors + 1);
			indices.push_back(v2);
		}
		//for top stack also only single triangle required
		if (j != Sectors - 1) {
			j != Sectors - 1 ? indices.push_back(v1 + 1) : indices.push_back(v1 - Sectors + 1);
			indices.push_back(v2);
			j != Sectors - 1 ? indices.push_back(v2 + 1) : indices.push_back(v2 - Sectors + 1);
		}
		v1++, v2++;
	}
}
````

### Scope for optimization

A sphere with 1000 stacks and 1000 sectors will have (1000 + 1) * (1000 + 1) vertices because there is one more vertex at the top and bottom of the sphere. Therefore, the total number of vertices would be 1,001 * 1,001 = 1,002,001.

Each stack, except for the top and bottom stacks, has two triangles per sector. Since there are 1000 sectors, there will be 2 * 1000 = 2000 triangles per stack. The top and bottom stacks have only one triangle per sector, which gives us 1000 triangles per stack. Therefore, the total number of triangles in the sphere mesh would be 2000 * 998 + 1000 * 2 = 1,994,000.

Since each triangle has three vertices, the total number of polygons in the sphere mesh would be 1,994,000 * 3 = 5,982,000.

So for 5,982,000 polygons the benchmarked time for above algorithm is 
````
Render time - 19.70s Stacks and Sectors = 1000
````
