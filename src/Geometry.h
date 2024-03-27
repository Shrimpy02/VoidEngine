#ifndef GEOMETRY_H
#define GEOMETRY_H

// GL Components
#include <algorithm>
#include <vector>
#include <glad/glad.h>

struct Vertex
{
	float x, y, z; // Position
	float nx, ny, nz; // normal vector
	float r, g, b; // Color
};

struct Normal
{
	float x, y, z; // Position
	float r, g, b; // Color
};


class Geometry
{
public:
	// ---------- Global Constants --------------



private:

	// ---------- Local Constants --------------

	unsigned int VAO, VBO, EBO;

	int numVertices = 0;

	bool doneonce = false;

	// ---------- Global functions --------------

public:
	Geometry()
	{

	}

	~Geometry()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	void Initialize(vector<float>* cube,int _numVariables)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		numVertices = cube->size() / _numVariables;

		switch (_numVariables)
		{
		case 3:
			bindBufferData(cube);
			createAttributePointers(true);
			break;
		case 6:
			bindBufferData(cube);
			createAttributePointers(true, true);
			break;
		case 9:
			bindBufferData(cube);
			createAttributePointers(true, true,true);
			break;
		default:
			cout << "ERROR NUM VARIABLES NOT CORRECT \n";
			break;
		}
	}

	void drawVertexGeometry(bool _useIndices, Shader* shaderToUse)
	{
		glBindVertexArray(VAO);
		shaderToUse->use();
		shaderToUse->processTransformations();

		if (_useIndices)
			glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, numVertices);

		glBindVertexArray(0);
	}

	void drawVertexGeometry(bool _useIndices, Shader* shaderToUse, vec3 positionOffset)
	{
		glBindVertexArray(VAO);
		shaderToUse->use();
		shaderToUse->processTransformations(positionOffset);

		if (_useIndices)
			glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, numVertices);

		glBindVertexArray(0);
	}

	void drawVertexGeometry(bool _useIndices, Shader* shaderToUse, vec3 positionOffset, vec3 scaleOffset)
	{
		glBindVertexArray(VAO);
		shaderToUse->use();
		shaderToUse->processTransformations(positionOffset, scaleOffset);

		if (_useIndices)
			glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, numVertices);

		glBindVertexArray(0);
	}

	void drawVertexGeometry(bool _useIndices, Shader* shaderToUse, vec3 positionOffset, vec3 scaleOffset, float rotationOffset, vec3 rotationAxis)
	{
		glBindVertexArray(VAO);
		shaderToUse->use();
		shaderToUse->processTransformations(positionOffset, scaleOffset, rotationOffset, rotationAxis);

		if (_useIndices)
			glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, numVertices);

		glBindVertexArray(0);
	}

	void drawNormalGeometry(Shader* shaderToUse, vec3 positionOffset, vec3 scaleOffset, float rotationOffset, vec3 rotationAxis)
	{
		glBindVertexArray(VAO);
		shaderToUse->use();
		shaderToUse->processTransformations(positionOffset, scaleOffset, rotationOffset, rotationAxis);

		glDrawArrays(GL_LINES, 0, numVertices);

		glBindVertexArray(0);
	}


	vector<float>* generateVertexGeometryFromFile(const char* _filePath)
	{
		return genVerticesFromStruct(readVerticesFromFile(_filePath));
	}

	vector<float>* generateNormalGeometryFromFile(const char* _filePath)
	{
		return genNormalsFromStruct(readNormalsFromFile(_filePath));
	}

private:

	// ---------- Local functions --------------

	vector<float>* genVerticesFromStruct(vector<Vertex*> _vertices)
	{
		vector<float>* temp = new vector<float>;
		for(int i = 0 ; i < _vertices.size(); i++)
		{
			temp->push_back(_vertices[i]->x);
			temp->push_back(_vertices[i]->y);
			temp->push_back(_vertices[i]->z);
			temp->push_back(_vertices[i]->nx);
			temp->push_back(_vertices[i]->ny);
			temp->push_back(_vertices[i]->nz);
			temp->push_back(_vertices[i]->r);
			temp->push_back(_vertices[i]->g);
			temp->push_back(_vertices[i]->b);
		}
		return temp;
	}

	vector<float>* genNormalsFromStruct(vector<Normal*> _normals)
	{
		vector<float>* temp = new vector<float>;
		for (int i = 0; i < _normals.size(); i++)
		{
			temp->push_back(_normals[i]->x);
			temp->push_back(_normals[i]->y);
			temp->push_back(_normals[i]->z);
			temp->push_back(_normals[i]->r);
			temp->push_back(_normals[i]->g);
			temp->push_back(_normals[i]->b);
		}
		return temp;
	}

	vector<unsigned int> GenerateIndices(vector<Vertex*> _vertices)
	{
		vector<unsigned int> temp;
		for(int i = 0; i < _vertices.size(); i++)
		{
			
		}
		return temp;
	}

	vector<Vertex*> readVerticesFromFile(const char* _filePath)
	{
		ifstream GeometryFile;
		// Open the txt file containing geometry data.
		GeometryFile.open(_filePath);

		// First line defines how many lines there are total
		// Extract and convert to int
		string numLinesStr;
		getline(GeometryFile, numLinesStr);
		int numLinesInt = stoi(numLinesStr);

		string numVariablesStr;
		getline(GeometryFile, numVariablesStr);
		int numVariablesInt = stoi(numVariablesStr);

		// Create a vector to contain all vertexes generated. 
		vector<Vertex*> temp;

		// Iterates through the total number of lines. 
		for (int i = 0; i < numLinesInt; i++)
		{
			// Gets the first line as a string, removes all spaces and parentheses
			string tempLines, result1, result2, result3;
			getline(GeometryFile, tempLines);
			std::remove_copy(tempLines.begin(), tempLines.end(), std::back_inserter(result1), ' ');
			std::remove_copy(result1.begin(), result1.end(), std::back_inserter(result2), '(');
			std::remove_copy(result2.begin(), result2.end(), std::back_inserter(result3), ')');

			// Pre defines variables for converting from string to float.
			float x = 0, y = 0, z = 0, nx = 0, ny = 0, nz = 0, r = 0, g = 0, b = 0;

			// Iterates through number of variables in current line
			for (int i = 0; i < numVariablesInt; i++)
			{
				// Gets the index between start and first number end defined by ','.
				int lastPosNumIndex = result3.find(',');
				// Copies this value into sting
				string final(result3.begin(), result3.begin() + lastPosNumIndex);
				// Erases that value from complete string
				result3.erase(result3.begin(), result3.begin() + lastPosNumIndex + 1);

				// Switch case to convert value to proper variable
				switch (i + 1)
				{
				case 1:
					x = stof(final);
					break;
				case 2:
					y = stof(final);
					break;
				case 3:
					z = stof(final);
					break;
				case 4:
					nx = stof(final);
					break;
				case 5:
					ny = stof(final);
					break;
				case 6:
					nz = stof(final);
					break;
				case 7:
					r = stof(final);
					break;
				case 8:
					g = stof(final);
					break;
				case 9:
					b = stof(final);
					break;
				default:
					// Error handling if default case triggered. 
					cout << "ERROR::READING_GEOMETRY_FILE::NO_MORE_VERTEX_ATTRIBUTE_LINES";
					break;
				}

			}

			// creates the new vertex with the given variables.
			temp.push_back(CreateNewVertex(x, y, z, nx, ny, nz, r, g, b));
		}

		// close file when finished reading it
		GeometryFile.close();

		return temp;
	}

	vector<Normal*> readNormalsFromFile(const char* _filePath)
	{
		ifstream GeometryFile;
		// Open the txt file containing geometry data.
		GeometryFile.open(_filePath);

		// First line defines how many lines there are total
		// Extract and convert to int
		string numLinesStr;
		getline(GeometryFile, numLinesStr);
		int numLinesInt = stoi(numLinesStr);

		string numVariablesStr;
		getline(GeometryFile, numVariablesStr);
		int numVariablesInt = stoi(numVariablesStr);

		// Create a vector to contain all vertexes generated. 
		vector<Normal*> temp;

		// Iterates through the total number of lines. 
		for (int i = 0; i < numLinesInt; i++)
		{
			// Gets the first line as a string, removes all spaces and parentheses
			string tempLines, result1, result2, result3;
			getline(GeometryFile, tempLines);
			std::remove_copy(tempLines.begin(), tempLines.end(), std::back_inserter(result1), ' ');
			std::remove_copy(result1.begin(), result1.end(), std::back_inserter(result2), '(');
			std::remove_copy(result2.begin(), result2.end(), std::back_inserter(result3), ')');

			// Pre defines variables for converting from string to float.
			float x = 0, y = 0, z = 0, r = 0, g = 0, b = 0;

			// Iterates through number of variables in current line
			for (int i = 0; i < numVariablesInt; i++)
			{
				// Gets the index between start and first number end defined by ','.
				int lastPosNumIndex = result3.find(',');
				// Copies this value into sting
				string final(result3.begin(), result3.begin() + lastPosNumIndex);
				// Erases that value from complete string
				result3.erase(result3.begin(), result3.begin() + lastPosNumIndex + 1);

				// Switch case to convert value to proper variable
				switch (i + 1)
				{
				case 1:
					x = stof(final);
					break;
				case 2:
					y = stof(final);
					break;
				case 3:
					z = stof(final);
					break;
				case 4:
					r = stof(final);
					break;
				case 5:
					g = stof(final);
					break;
				case 6:
					b = stof(final);
					break;
				default:
					// Error handling if default case triggered. 
					cout << "ERROR::READING_GEOMETRY_FILE::NO_MORE_VERTEX_ATTRIBUTE_LINES";
					break;
				}

			}

			// creates the new vertex with the given variables.
			temp.push_back(CreateNormal(x, y, z, r, g, b));
		}

		// close file when finished reading it
		GeometryFile.close();

		return temp;
	}

	void bindBufferData(vector<float>* _vertices)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, _vertices->size() * sizeof(float), _vertices->data(), GL_STATIC_DRAW);
	}

	void bindBufferData(vector<float> _vertices, vector<unsigned int> _indices)
	{

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(_vertices.data()), _vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(_indices.data()), _indices.data(), GL_STATIC_DRAW);

	}

	void createAttributePointers(bool pos)
	{
		// position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	
	}

	void createAttributePointers(bool pos,bool color)
	{
		// position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	void createAttributePointers(bool pos, bool normals, bool color)
	{
		// position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// color
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	Vertex* CreateNewVertex(float _x, float _y, float _z, float _nx, float _ny, float _nz, float _r, float _g, float _b)
	{
		Vertex* NewVertex = new Vertex();
		NewVertex->x = _x;
		NewVertex->y = _y;
		NewVertex->z = _z;
		NewVertex->nx = _nx;
		NewVertex->ny = _ny;
		NewVertex->nz = _nz;
		NewVertex->r = _r;
		NewVertex->g = _g;
		NewVertex->b = _b;
		return NewVertex;
	}

	Normal* CreateNormal(float _x, float _y, float _z, float _r, float _g, float _b)
	{
		Normal* NewNormal = new Normal();
		NewNormal->x = _x;
		NewNormal->y = _y;
		NewNormal->z = _z;
		NewNormal->r = _r;
		NewNormal->g = _g;
		NewNormal->b = _b;
		return NewNormal;
	}

	void writeVertexToConsole(vector<Vertex*> vectorToPrint)
	{
		for (int i = 0; i < vectorToPrint.size(); i++)
		{
			cout << "x = " << vectorToPrint[i]->x << ", ";
			cout << "y = " << vectorToPrint[i]->y << ", ";
			cout << "z = " << vectorToPrint[i]->z << ", ";
			cout << "r = " << vectorToPrint[i]->r << ", ";
			cout << "g = " << vectorToPrint[i]->g << ", ";
			cout << "b = " << vectorToPrint[i]->b << ", ";
			cout << endl;
		}
	}

	// ---------- Getters and setters --------------
public:


};



#endif
