// Vila Virtual - Computação Gráfica
//
// Projeto com:
// - Terreno
// - Casas com telhados triangulares
// - Árvores
// - Torre
// - Poço
// - Pedras
// - Cercas
// - Estrada
// - Câmera 3D navegável
// - Projeção perspectiva
//
// Geometrias utilizadas:
// - Cubo
// - Prisma triangular

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// ============================================================
// CONFIGURAÇÕES
// ============================================================

GLFWwindow *Window = nullptr;

GLuint Shader_programm = 0;

GLuint VaoCubo = 0;
GLuint VboCubo = 0;

GLuint VaoTerreno = 0;
GLuint VboTerreno = 0;
GLuint EboTerreno = 0;
GLuint texturaGrama = 0;

GLuint VaoSeta = 0;
GLuint VboSeta = 0;

GLuint VaoTelhado = 0;
GLuint VboTelhado = 0;

GLint ModelLoc = -1;
GLint ObjectColorLoc = -1;

GLint UsarTexturaLoc = -1;
GLint TexturaLoc = -1;

int WIDTH = 1000;
int HEIGHT = 700;

float Tempo_entre_frames = 0.0f;

float Cam_speed = 8.0f;

bool mostrarMinimapa = true;
bool teclaMPressionada = false;

glm::vec3 Cam_pos =
    glm::vec3(
        0.0f,
        8.0f,
        25.0f);

glm::vec3 Cam_front =
    glm::vec3(
        0.0f,
        -0.15f,
        -1.0f);

glm::vec3 Cam_up =
    glm::vec3(
        0.0f,
        1.0f,
        0.0f);

float Cam_yaw = 0.0f;
float Cam_pitch = -10.0f;

double lastX = WIDTH / 2.0;
double lastY = HEIGHT / 2.0;

bool primeiro_mouse = true;

// ============================================================
// CORES
// ============================================================

const glm::vec3 COR_GRAMA =
    glm::vec3(
        0.18f,
        0.42f,
        0.16f);

const glm::vec3 COR_ESTRADA =
    glm::vec3(
        0.32f,
        0.27f,
        0.20f);

const glm::vec3 COR_CASA =
    glm::vec3(
        0.68f,
        0.52f,
        0.32f);

const glm::vec3 COR_CASA_CLARA =
    glm::vec3(
        0.78f,
        0.65f,
        0.43f);

const glm::vec3 COR_CASA_ESCURA =
    glm::vec3(
        0.48f,
        0.34f,
        0.22f);

const glm::vec3 COR_PORTA =
    glm::vec3(
        0.28f,
        0.12f,
        0.05f);

const glm::vec3 COR_JANELA =
    glm::vec3(
        0.12f,
        0.32f,
        0.55f);

const glm::vec3 COR_MADEIRA =
    glm::vec3(
        0.36f,
        0.18f,
        0.07f);

const glm::vec3 COR_FOLHAS =
    glm::vec3(
        0.12f,
        0.38f,
        0.10f);

const glm::vec3 COR_FOLHAS_CLARAS =
    glm::vec3(
        0.20f,
        0.48f,
        0.12f);

const glm::vec3 COR_PEDRA =
    glm::vec3(
        0.38f,
        0.38f,
        0.38f);

const glm::vec3 COR_PEDRA_CLARA =
    glm::vec3(
        0.50f,
        0.50f,
        0.48f);

const glm::vec3 COR_TORRE =
    glm::vec3(
        0.42f,
        0.42f,
        0.40f);

const glm::vec3 COR_TELHADO =
    glm::vec3(
        0.48f,
        0.16f,
        0.08f);

const glm::vec3 COR_TELHADO_CLARO =
    glm::vec3(
        0.62f,
        0.22f,
        0.10f);

// ============================================================
// LEITURA DE SHADER
// ============================================================

std::string leShaderDoArquivo(const char *caminhoArquivo)
{
    std::ifstream arquivoShader(caminhoArquivo);

    // Verifica se conseguiu abrir o arquivo
    if (!arquivoShader.is_open())
    {
        std::cerr << "ERRO: Nao foi possivel abrir o arquivo do shader: " << caminhoArquivo << std::endl;
        return "";
    }

    std::stringstream shaderStream;
    // Lê o buffer do arquivo e joga no stream
    shaderStream << arquivoShader.rdbuf();
    arquivoShader.close();

    // Retorna o stream convertido para string
    return shaderStream.str();
}

// ============================================================
// COMPILAR SHADER
// ============================================================

GLuint compilaShader(
    const std::string &codigo,
    GLenum tipo)
{
    GLuint shader =
        glCreateShader(tipo);

    const char *fonte =
        codigo.c_str();

    glShaderSource(
        shader,
        1,
        &fonte,
        nullptr);

    glCompileShader(shader);

    GLint sucesso = 0;

    glGetShaderiv(
        shader,
        GL_COMPILE_STATUS,
        &sucesso);

    if (!sucesso)
    {
        char log[1024];

        glGetShaderInfoLog(
            shader,
            1024,
            nullptr,
            log);

        std::cerr
            << "Erro compilando shader:\n"
            << log
            << std::endl;
    }

    return shader;
}

// ============================================================
// INICIALIZA SHADERS
// ============================================================

void inicializaShaders()
{
    // ========================================================
    // 1. Lê os arquivos dos shaders
    // ========================================================

    std::string vertexCode =
        leShaderDoArquivo(
            "../assets/shaders/vertex_shader.glsl");

    std::string fragmentCode =
        leShaderDoArquivo(
            "../assets/shaders/fragment_shader.glsl");

    // Caso o executável esteja sendo executado
    // a partir de build/Debug.
    if (
        vertexCode.empty() ||
        fragmentCode.empty())
    {
        vertexCode =
            leShaderDoArquivo(
                "../../assets/shaders/vertex_shader.glsl");

        fragmentCode =
            leShaderDoArquivo(
                "../../assets/shaders/fragment_shader.glsl");
    }

    // ========================================================
    // 2. Verifica se os arquivos foram carregados
    // ========================================================

    if (
        vertexCode.empty() ||
        fragmentCode.empty())
    {
        std::cerr
            << "ERRO: Nao foi possivel carregar os shaders."
            << std::endl;

        return;
    }

    // ========================================================
    // 3. Converte std::string para const char*
    // ========================================================

    const char *vertex_shader =
        vertexCode.c_str();

    const char *fragment_shader =
        fragmentCode.c_str();

    // ========================================================
    // 4. Vertex Shader
    // ========================================================

    GLuint vs =
        glCreateShader(
            GL_VERTEX_SHADER);

    glShaderSource(
        vs,
        1,
        &vertex_shader,
        nullptr);

    glCompileShader(vs);

    GLint success = 0;

    char infoLog[512];

    glGetShaderiv(
        vs,
        GL_COMPILE_STATUS,
        &success);

    if (!success)
    {
        glGetShaderInfoLog(
            vs,
            512,
            nullptr,
            infoLog);

        std::cerr
            << "Erro no vertex shader:\n"
            << infoLog
            << std::endl;
    }

    // ========================================================
    // 5. Fragment Shader
    // ========================================================

    GLuint fs =
        glCreateShader(
            GL_FRAGMENT_SHADER);

    glShaderSource(
        fs,
        1,
        &fragment_shader,
        nullptr);

    glCompileShader(fs);

    glGetShaderiv(
        fs,
        GL_COMPILE_STATUS,
        &success);

    if (!success)
    {
        glGetShaderInfoLog(
            fs,
            512,
            nullptr,
            infoLog);

        std::cerr
            << "Erro no fragment shader:\n"
            << infoLog
            << std::endl;
    }

    // ========================================================
    // 6. Shader Program
    // ========================================================

    Shader_programm =
        glCreateProgram();

    glAttachShader(
        Shader_programm,
        vs);

    glAttachShader(
        Shader_programm,
        fs);

    glLinkProgram(
        Shader_programm);

    glGetProgramiv(
        Shader_programm,
        GL_LINK_STATUS,
        &success);

    if (!success)
    {
        glGetProgramInfoLog(
            Shader_programm,
            512,
            nullptr,
            infoLog);

        std::cerr
            << "Erro na linkagem do shader:\n"
            << infoLog
            << std::endl;
    }

    // ========================================================
    // 7. Remove shaders temporários
    // ========================================================

    glDeleteShader(vs);
    glDeleteShader(fs);

    // ========================================================
    // 8. Localiza os uniforms
    // ========================================================

    ModelLoc =
        glGetUniformLocation(
            Shader_programm,
            "model");

    ObjectColorLoc =
        glGetUniformLocation(
            Shader_programm,
            "objectColor");

    UsarTexturaLoc =
        glGetUniformLocation(
            Shader_programm,
            "usarTextura");

    TexturaLoc =
        glGetUniformLocation(
            Shader_programm,
            "textura");
}

void inicializaTerreno()
{
    float vertices[] = {
        // X       Y     Z        U      V
        -30.0f, 0.0f, -30.0f, 0.0f, 0.0f, // 0
        30.0f, 0.0f, -30.0f, 10.0f, 0.0f, // 1
        30.0f, 0.0f, 30.0f, 10.0f, 10.0f, // 2
        -30.0f, 0.0f, 30.0f, 0.0f, 10.0f  // 3
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3};

    glGenVertexArrays(1, &VaoTerreno);
    glGenBuffers(1, &VboTerreno);
    glGenBuffers(1, &EboTerreno);

    glBindVertexArray(VaoTerreno);

    // VBO
    glBindBuffer(
        GL_ARRAY_BUFFER,
        VboTerreno);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW);

    // EBO
    glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        EboTerreno);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices,
        GL_STATIC_DRAW);

    // Posição
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (void *)0);

    glEnableVertexAttribArray(0);

    // UV
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (void *)(3 * sizeof(float)));

    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void carregaTexturaGrama()
{
    glGenTextures(1, &texturaGrama);
    glBindTexture(GL_TEXTURE_2D, texturaGrama);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_REPEAT);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_REPEAT);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    int largura = 0;
    int altura = 0;
    int canais = 0;

    const char *caminho =
        "../assets/tex/grass.jpg";

    unsigned char *dados =
        stbi_load(
            caminho,
            &largura,
            &altura,
            &canais,
            0);

    if (!dados)
    {
        std::cerr
            << "ERRO STB: "
            << stbi_failure_reason()
            << std::endl;

        return;
    }

    GLenum formato;

    if (canais == 1)
        formato = GL_RED;
    else if (canais == 3)
        formato = GL_RGB;
    else if (canais == 4)
        formato = GL_RGBA;
    else
    {
        std::cerr
            << "Quantidade de canais nao suportada: "
            << canais
            << std::endl;

        stbi_image_free(dados);
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        formato,
        largura,
        altura,
        0,
        formato,
        GL_UNSIGNED_BYTE,
        dados);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(dados);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void desenhaTerreno()
{
    glm::mat4 modelo =
        glm::mat4(1.0f);

    glUniformMatrix4fv(
        ModelLoc,
        1,
        GL_FALSE,
        glm::value_ptr(modelo));

    // Ativa textura no fragment shader
    glUniform1i(
        UsarTexturaLoc,
        GL_TRUE);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(
        GL_TEXTURE_2D,
        texturaGrama);

    glUniform1i(
        TexturaLoc,
        0);

    glBindVertexArray(
        VaoTerreno);

    glDrawElements(
        GL_TRIANGLES,
        6,
        GL_UNSIGNED_INT,
        0);

    glBindVertexArray(0);

    // Desativa novamente para os demais objetos
    glUniform1i(
        UsarTexturaLoc,
        GL_FALSE);
}

// ============================================================
// CUBO
// ============================================================

void inicializaCubo()
{
    float vertices[] =
        {
            // Frente
            -0.5f, -0.5f, 0.5f,
            0.5f, -0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,

            0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f,

            // Trás
            -0.5f, -0.5f, -0.5f,
            -0.5f, 0.5f, -0.5f,
            0.5f, 0.5f, -0.5f,

            0.5f, 0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,

            // Esquerda
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f,

            -0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,

            // Direita
            0.5f, -0.5f, -0.5f,
            0.5f, 0.5f, -0.5f,
            0.5f, 0.5f, 0.5f,

            0.5f, 0.5f, 0.5f,
            0.5f, -0.5f, 0.5f,
            0.5f, -0.5f, -0.5f,

            // Cima
            -0.5f, 0.5f, -0.5f,
            -0.5f, 0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,

            0.5f, 0.5f, 0.5f,
            0.5f, 0.5f, -0.5f,
            -0.5f, 0.5f, -0.5f,

            // Baixo
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, 0.5f,

            0.5f, -0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f,
            -0.5f, -0.5f, -0.5f};

    glGenVertexArrays(
        1,
        &VaoCubo);

    glGenBuffers(
        1,
        &VboCubo);

    glBindVertexArray(
        VaoCubo);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        VboCubo);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        nullptr);

    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

// ============================================================
// PRISMA TRIANGULAR
//
// O prisma representa um telhado.
// A seção transversal é um triângulo:
//
//       /\
//      /  \
//     /____\
//
// Ele possui comprimento no eixo Z.
// ============================================================

void inicializaTelhado()
{
    float vertices[] =
        {
            // Frente
            // base esquerda
            -0.5f, 0.0f, 0.5f,

            // base direita
            0.5f, 0.0f, 0.5f,

            // topo
            0.0f, 0.8f, 0.5f,

            // Trás
            -0.5f, 0.0f, -0.5f,

            0.0f, 0.8f, -0.5f,

            0.5f, 0.0f, -0.5f,

            // Lado esquerdo
            -0.5f, 0.0f, 0.5f,
            -0.5f, 0.0f, -0.5f,
            0.0f, 0.8f, -0.5f,

            0.0f, 0.8f, -0.5f,
            0.0f, 0.8f, 0.5f,
            -0.5f, 0.0f, 0.5f,

            // Lado direito
            0.5f, 0.0f, 0.5f,
            0.0f, 0.8f, 0.5f,
            0.0f, 0.8f, -0.5f,

            0.0f, 0.8f, -0.5f,
            0.5f, 0.0f, -0.5f,
            0.5f, 0.0f, 0.5f,

            // Base
            -0.5f, 0.0f, -0.5f,
            -0.5f, 0.0f, 0.5f,
            0.5f, 0.0f, 0.5f,

            0.5f, 0.0f, 0.5f,
            0.5f, 0.0f, -0.5f,
            -0.5f, 0.0f, -0.5f};

    glGenVertexArrays(
        1,
        &VaoTelhado);

    glGenBuffers(
        1,
        &VboTelhado);

    glBindVertexArray(
        VaoTelhado);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        VboTelhado);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        nullptr);

    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void inicializaSeta()
{
    float vertices[] =
        {
            // Triângulo no plano XZ
            0.0f, 0.0f, -1.5f,
            -0.8f, 0.0f, 0.8f,
            0.8f, 0.0f, 0.8f};

    glGenVertexArrays(
        1,
        &VaoSeta);

    glGenBuffers(
        1,
        &VboSeta);

    glBindVertexArray(
        VaoSeta);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        VboSeta);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        nullptr);

    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

// ============================================================
// DESENHAR CUBO
// ============================================================

void desenhaCubo(
    const glm::mat4 &transformacao,
    const glm::vec3 &escala,
    const glm::vec3 &cor)
{
    glUniform1i(
        UsarTexturaLoc,
        GL_FALSE);

    glm::mat4 modelo =
        glm::scale(
            transformacao,
            escala);

    glUniformMatrix4fv(
        ModelLoc,
        1,
        GL_FALSE,
        glm::value_ptr(modelo));

    glUniform3fv(
        ObjectColorLoc,
        1,
        glm::value_ptr(cor));

    glBindVertexArray(
        VaoCubo);

    glDrawArrays(
        GL_TRIANGLES,
        0,
        36);
}

// ============================================================
// DESENHAR TELHADO
// ============================================================

void desenhaTelhado(
    const glm::mat4 &transformacao,
    const glm::vec3 &escala,
    const glm::vec3 &cor)
{
    glUniform1i(
        UsarTexturaLoc,
        GL_FALSE);

    glm::mat4 modelo =
        glm::scale(
            transformacao,
            escala);

    glUniformMatrix4fv(
        ModelLoc,
        1,
        GL_FALSE,
        glm::value_ptr(modelo));

    glUniform3fv(
        ObjectColorLoc,
        1,
        glm::value_ptr(cor));

    glBindVertexArray(
        VaoTelhado);

    glDrawArrays(
        GL_TRIANGLES,
        0,
        24);
}

// ============================================================
// DESENHAR CASA
// ============================================================

void desenhaCasa(
    const glm::vec3 &posicao,
    float tamanho,
    float rotacao,
    const glm::vec3 &corCasa,
    const glm::vec3 &corTelhado)
{
    glm::mat4 transformacao =
        glm::mat4(1.0f);

    transformacao =
        glm::translate(
            transformacao,
            posicao);

    transformacao =
        glm::rotate(
            transformacao,
            glm::radians(rotacao),
            glm::vec3(
                0.0f,
                1.0f,
                0.0f));

    // --------------------------------------------------------
    // CORPO
    // --------------------------------------------------------

    glm::mat4 modelo =
        glm::translate(
            transformacao,
            glm::vec3(
                0.0f,
                tamanho,
                0.0f));

    desenhaCubo(
        modelo,
        glm::vec3(
            tamanho * 2.0f,
            tamanho * 2.0f,
            tamanho * 2.0f),
        corCasa);

    // --------------------------------------------------------
    // TELHADO
    // --------------------------------------------------------

    modelo =
        glm::translate(
            transformacao,
            glm::vec3(
                0.0f,
                tamanho * 2.0f,
                0.0f));

    desenhaTelhado(
        modelo,
        glm::vec3(
            tamanho * 2.35f,
            tamanho * 1.45f,
            tamanho * 2.30f),
        corTelhado);

    // --------------------------------------------------------
    // PORTA
    // --------------------------------------------------------

    modelo =
        glm::translate(
            transformacao,
            glm::vec3(
                0.0f,
                tamanho * 0.5f,
                tamanho + 0.06f));

    desenhaCubo(
        modelo,
        glm::vec3(
            tamanho * 0.50f,
            tamanho * 1.0f,
            tamanho * 0.10f),
        COR_PORTA);

    // --------------------------------------------------------
    // JANELA ESQUERDA
    // --------------------------------------------------------

    modelo =
        glm::translate(
            transformacao,
            glm::vec3(
                -tamanho * 0.65f,
                tamanho * 1.30f,
                tamanho + 0.06f));

    desenhaCubo(
        modelo,
        glm::vec3(
            tamanho * 0.45f,
            tamanho * 0.45f,
            tamanho * 0.10f),
        COR_JANELA);

    // --------------------------------------------------------
    // JANELA DIREITA
    // --------------------------------------------------------

    modelo =
        glm::translate(
            transformacao,
            glm::vec3(
                tamanho * 0.65f,
                tamanho * 1.30f,
                tamanho + 0.06f));

    desenhaCubo(
        modelo,
        glm::vec3(
            tamanho * 0.45f,
            tamanho * 0.45f,
            tamanho * 0.10f),
        COR_JANELA);

    // --------------------------------------------------------
    // CHAMINÉ
    // --------------------------------------------------------

    modelo =
        glm::translate(
            transformacao,
            glm::vec3(
                tamanho * 0.55f,
                tamanho * 2.25f,
                -tamanho * 0.25f));

    desenhaCubo(
        modelo,
        glm::vec3(
            tamanho * 0.30f,
            tamanho * 0.70f,
            tamanho * 0.30f),
        COR_TORRE);
}

// ============================================================
// ÁRVORE
// ============================================================

void desenhaArvore(
    const glm::vec3 &posicao,
    float tamanho,
    bool clara)
{
    glm::mat4 transformacao =
        glm::translate(
            glm::mat4(1.0f),
            posicao);

    // Tronco
    glm::mat4 tronco =
        glm::translate(
            transformacao,
            glm::vec3(
                0.0f,
                tamanho * 0.75f,
                0.0f));

    desenhaCubo(
        tronco,
        glm::vec3(
            tamanho * 0.35f,
            tamanho * 1.5f,
            tamanho * 0.35f),
        COR_MADEIRA);

    // Copa inferior
    glm::mat4 copa =
        glm::translate(
            transformacao,
            glm::vec3(
                0.0f,
                tamanho * 1.65f,
                0.0f));

    desenhaTelhado(
        copa,
        glm::vec3(
            tamanho * 1.7f,
            tamanho * 1.4f,
            tamanho * 1.7f),
        clara
            ? COR_FOLHAS_CLARAS
            : COR_FOLHAS);

    // Copa superior
    copa =
        glm::translate(
            transformacao,
            glm::vec3(
                0.0f,
                tamanho * 2.20f,
                0.0f));

    desenhaTelhado(
        copa,
        glm::vec3(
            tamanho * 1.3f,
            tamanho * 1.15f,
            tamanho * 1.3f),
        clara
            ? COR_FOLHAS_CLARAS
            : COR_FOLHAS);
}

// ============================================================
// TORRE
// ============================================================

void desenhaTorre(
    const glm::vec3 &posicao)
{
    glm::mat4 transformacao =
        glm::translate(
            glm::mat4(1.0f),
            posicao);

    // Base
    glm::mat4 modelo =
        glm::translate(
            transformacao,
            glm::vec3(
                0.0f,
                3.0f,
                0.0f));

    desenhaCubo(
        modelo,
        glm::vec3(
            5.0f,
            6.0f,
            5.0f),
        COR_TORRE);

    // Telhado
    modelo =
        glm::translate(
            transformacao,
            glm::vec3(
                0.0f,
                6.0f,
                0.0f));

    desenhaTelhado(
        modelo,
        glm::vec3(
            5.7f,
            3.0f,
            5.7f),
        COR_TELHADO);

    // Janela frontal
    modelo =
        glm::translate(
            transformacao,
            glm::vec3(
                0.0f,
                3.5f,
                2.55f));

    desenhaCubo(
        modelo,
        glm::vec3(
            1.0f,
            1.2f,
            0.15f),
        COR_JANELA);

    // Porta
    modelo =
        glm::translate(
            transformacao,
            glm::vec3(
                0.0f,
                1.0f,
                2.55f));

    desenhaCubo(
        modelo,
        glm::vec3(
            1.4f,
            2.0f,
            0.15f),
        COR_PORTA);
}

// ============================================================
// POÇO
// ============================================================

void desenhaPoco(
    const glm::vec3 &posicao)
{
    glm::mat4 transformacao =
        glm::translate(
            glm::mat4(1.0f),
            posicao);

    // Quatro blocos formando o poço

    glm::mat4 bloco =
        glm::translate(
            transformacao,
            glm::vec3(
                -1.0f,
                0.5f,
                0.0f));

    desenhaCubo(
        bloco,
        glm::vec3(
            0.5f,
            1.0f,
            2.5f),
        COR_PEDRA);

    bloco =
        glm::translate(
            transformacao,
            glm::vec3(
                1.0f,
                0.5f,
                0.0f));

    desenhaCubo(
        bloco,
        glm::vec3(
            0.5f,
            1.0f,
            2.5f),
        COR_PEDRA_CLARA);

    bloco =
        glm::translate(
            transformacao,
            glm::vec3(
                0.0f,
                0.5f,
                -1.0f));

    desenhaCubo(
        bloco,
        glm::vec3(
            1.5f,
            1.0f,
            0.5f),
        COR_PEDRA);

    bloco =
        glm::translate(
            transformacao,
            glm::vec3(
                0.0f,
                0.5f,
                1.0f));

    desenhaCubo(
        bloco,
        glm::vec3(
            1.5f,
            1.0f,
            0.5f),
        COR_PEDRA_CLARA);

    // Poste
    bloco =
        glm::translate(
            transformacao,
            glm::vec3(
                0.0f,
                2.5f,
                -0.8f));

    desenhaCubo(
        bloco,
        glm::vec3(
            0.25f,
            4.0f,
            0.25f),
        COR_MADEIRA);

    // Travessa
    bloco =
        glm::translate(
            transformacao,
            glm::vec3(
                0.0f,
                3.8f,
                0.0f));

    desenhaCubo(
        bloco,
        glm::vec3(
            2.0f,
            0.25f,
            0.25f),
        COR_MADEIRA);
}

// ============================================================
// MOINHO
// ============================================================

void desenhaMoinho(const glm::vec3 &posicao)
{
    glm::mat4 base =
        glm::translate(
            glm::mat4(1.0f),
            posicao);

    // Corpo do moinho
    glm::mat4 modelo =
        glm::translate(
            base,
            glm::vec3(0.0f, 4.0f, 0.0f));

    desenhaCubo(
        modelo,
        glm::vec3(3.0f, 8.0f, 3.0f),
        COR_CASA_CLARA);

    // Centro das pás
    glm::mat4 centro =
        glm::translate(
            base,
            glm::vec3(0.0f, 6.0f, 1.6f));

    // Rotação baseada no tempo
    float angulo =
        static_cast<float>(glfwGetTime()) * 60.0f;

    centro =
        glm::rotate(
            centro,
            glm::radians(angulo),
            glm::vec3(0.0f, 0.0f, 1.0f));

    // Pá vertical
    desenhaCubo(
        centro,
        glm::vec3(0.35f, 5.0f, 0.20f),
        COR_MADEIRA);

    // Pá horizontal
    glm::mat4 paHorizontal =
        glm::rotate(
            centro,
            glm::radians(90.0f),
            glm::vec3(0.0f, 0.0f, 1.0f));

    desenhaCubo(
        paHorizontal,
        glm::vec3(0.35f, 5.0f, 0.20f),
        COR_MADEIRA);
}

// ============================================================
// PEDRA
// ============================================================

void desenhaPedra(
    const glm::vec3 &posicao,
    float tamanho)
{
    glm::mat4 modelo =
        glm::translate(
            glm::mat4(1.0f),
            posicao);

    desenhaCubo(
        modelo,
        glm::vec3(
            tamanho,
            tamanho * 0.6f,
            tamanho * 0.8f),
        COR_PEDRA);
}

// ============================================================
// CERCA
// ============================================================

void desenhaCerca(
    const glm::vec3 &posicao,
    int quantidade,
    float espacamento,
    bool horizontal)
{
    for (int i = 0; i < quantidade; i++)
    {
        float deslocamento =
            (static_cast<float>(i) -
             static_cast<float>(quantidade - 1) / 2.0f) *
            espacamento;

        glm::vec3 offset;

        if (horizontal)
        {
            offset =
                glm::vec3(
                    deslocamento,
                    0.8f,
                    0.0f);
        }
        else
        {
            offset =
                glm::vec3(
                    0.0f,
                    0.8f,
                    deslocamento);
        }

        glm::mat4 modelo =
            glm::translate(
                glm::mat4(1.0f),
                posicao + offset);

        desenhaCubo(
            modelo,
            glm::vec3(
                0.25f,
                1.6f,
                0.25f),
            COR_MADEIRA);
    }
}

// ============================================================
// ESTRADA
// ============================================================

void desenhaEstrada()
{
    glm::mat4 modelo =
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(
                0.0f,
                0.03f,
                0.0f));

    desenhaCubo(
        modelo,
        glm::vec3(
            5.0f,
            0.08f,
            55.0f),
        COR_ESTRADA);

    modelo =
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(
                0.0f,
                0.04f,
                0.0f));

    desenhaCubo(
        modelo,
        glm::vec3(
            55.0f,
            0.08f,
            5.0f),
        COR_ESTRADA);
}

// ============================================================
// VILA
// ============================================================

void desenhaVila()
{
    // --------------------------------------------------------
    // TERRENO
    // --------------------------------------------------------

    desenhaTerreno();

    // --------------------------------------------------------
    // ESTRADAS
    // --------------------------------------------------------

    desenhaEstrada();

    // --------------------------------------------------------
    // CASAS
    // --------------------------------------------------------

    desenhaCasa(
        glm::vec3(
            -14.0f,
            0.0f,
            -14.0f),
        2.0f,
        20.0f,
        COR_CASA,
        COR_TELHADO);

    desenhaCasa(
        glm::vec3(
            14.0f,
            0.0f,
            -14.0f),
        2.2f,
        -20.0f,
        COR_CASA_CLARA,
        COR_TELHADO_CLARO);

    desenhaCasa(
        glm::vec3(
            -14.0f,
            0.0f,
            14.0f),
        1.8f,
        -15.0f,
        COR_CASA_ESCURA,
        COR_TELHADO);

    desenhaCasa(
        glm::vec3(
            14.0f,
            0.0f,
            14.0f),
        2.0f,
        25.0f,
        COR_CASA,
        COR_TELHADO_CLARO);

    desenhaCasa(
        glm::vec3(
            -24.0f,
            0.0f,
            0.0f),
        1.7f,
        90.0f,
        COR_CASA_CLARA,
        COR_TELHADO);

    desenhaCasa(
        glm::vec3(
            24.0f,
            0.0f,
            0.0f),
        1.9f,
        -90.0f,
        COR_CASA,
        COR_TELHADO);

    // --------------------------------------------------------
    // ÁRVORES
    // --------------------------------------------------------

    desenhaArvore(
        glm::vec3(
            -7.0f,
            0.0f,
            -18.0f),
        2.0f,
        false);

    desenhaArvore(
        glm::vec3(
            7.0f,
            0.0f,
            -18.0f),
        2.2f,
        true);

    desenhaArvore(
        glm::vec3(
            -20.0f,
            0.0f,
            -7.0f),
        1.8f,
        true);

    desenhaArvore(
        glm::vec3(
            20.0f,
            0.0f,
            -7.0f),
        2.0f,
        false);

    desenhaArvore(
        glm::vec3(
            -20.0f,
            0.0f,
            8.0f),
        2.1f,
        false);

    desenhaArvore(
        glm::vec3(
            20.0f,
            0.0f,
            8.0f),
        1.7f,
        true);

    desenhaArvore(
        glm::vec3(
            -7.0f,
            0.0f,
            20.0f),
        2.0f,
        true);

    desenhaArvore(
        glm::vec3(
            8.0f,
            0.0f,
            20.0f),
        2.2f,
        false);

    // --------------------------------------------------------
    // TORRE
    // --------------------------------------------------------

    desenhaTorre(
        glm::vec3(
            0.0f,
            0.0f,
            -15.0f));

    // --------------------------------------------------------
    // POÇO
    // --------------------------------------------------------

    desenhaPoco(
        glm::vec3(
            0.0f,
            0.0f,
            8.0f));

    // --------------------------------------------------------
    // MOINHO
    // --------------------------------------------------------

    desenhaMoinho(
        glm::vec3(
            18.0f,
            0.0f,
            -22.0f));

    // --------------------------------------------------------
    // PEDRAS
    // --------------------------------------------------------

    desenhaPedra(
        glm::vec3(
            -5.0f,
            0.2f,
            6.0f),
        1.3f);

    desenhaPedra(
        glm::vec3(
            5.0f,
            0.2f,
            6.5f),
        1.0f);

    desenhaPedra(
        glm::vec3(
            -5.0f,
            0.2f,
            11.0f),
        1.5f);

    desenhaPedra(
        glm::vec3(
            7.0f,
            0.2f,
            12.0f),
        1.2f);

    desenhaPedra(
        glm::vec3(
            -8.0f,
            0.2f,
            -8.0f),
        0.9f);

    // --------------------------------------------------------
    // CERCAS
    // --------------------------------------------------------

    desenhaCerca(
        glm::vec3(
            -20.0f,
            0.0f,
            15.0f),
        8,
        1.5f,
        true);

    desenhaCerca(
        glm::vec3(
            20.0f,
            0.0f,
            15.0f),
        8,
        1.5f,
        true);

    desenhaCerca(
        glm::vec3(
            -15.0f,
            0.0f,
            -22.0f),
        6,
        1.5f,
        true);
}

void desenhaPlayerNoMinimapa()
{
    glm::vec3 posicaoPlayer =
        glm::vec3(
            Cam_pos.x,
            0.5f,
            Cam_pos.z);

    glm::vec3 frenteHorizontal =
        glm::normalize(
            glm::vec3(
                Cam_front.x,
                0.0f,
                Cam_front.z));

    float angulo =
        std::atan2(
            frenteHorizontal.x,
            -frenteHorizontal.z);

    glm::mat4 model =
        glm::mat4(1.0f);

    model =
        glm::translate(
            model,
            posicaoPlayer);

    model =
        glm::rotate(
            model,
            angulo,
            glm::vec3(
                0.0f,
                1.0f,
                0.0f));

    glUniformMatrix4fv(
        ModelLoc,
        1,
        GL_FALSE,
        glm::value_ptr(model));

    glUniform3fv(
        ObjectColorLoc,
        1,
        glm::value_ptr(
            glm::vec3(
                0.9f,
                0.05f,
                0.05f)));

    glBindVertexArray(
        VaoSeta);

    glDrawArrays(
        GL_TRIANGLES,
        0,
        3);
}

void desenhaMinimapa()
{
    GLint viewport[4];

    glGetIntegerv(
        GL_VIEWPORT,
        viewport);

    int mapaLargura = 220;
    int mapaAltura = 220;

    int mapaX =
        WIDTH -
        mapaLargura -
        20;

    int mapaY =
        HEIGHT -
        mapaAltura -
        20;

    glViewport(
        mapaX,
        mapaY,
        mapaLargura,
        mapaAltura);

    glClear(
        GL_DEPTH_BUFFER_BIT);

    glm::mat4 projMapa =
        glm::ortho(
            -35.0f,
            35.0f,
            -35.0f,
            35.0f,
            0.1f,
            100.0f);

    glm::mat4 viewMapa =
        glm::lookAt(
            glm::vec3(
                0.0f,
                50.0f,
                0.0f),
            glm::vec3(
                0.0f,
                0.0f,
                0.0f),
            glm::vec3(
                0.0f,
                0.0f,
                -1.0f));

    GLint ViewLoc =
        glGetUniformLocation(
            Shader_programm,
            "view");

    GLint ProjLoc =
        glGetUniformLocation(
            Shader_programm,
            "proj");

    glUseProgram(
        Shader_programm);

    glUniformMatrix4fv(
        ViewLoc,
        1,
        GL_FALSE,
        glm::value_ptr(
            viewMapa));

    glUniformMatrix4fv(
        ProjLoc,
        1,
        GL_FALSE,
        glm::value_ptr(
            projMapa));

    desenhaVila();

    desenhaPlayerNoMinimapa();

    glViewport(
        viewport[0],
        viewport[1],
        viewport[2],
        viewport[3]);
}

// ============================================================
// CALLBACK DE TAMANHO DA JANELA
// ============================================================

void framebuffer_size_callback(
    GLFWwindow *window,
    int width,
    int height)
{
    WIDTH = width;
    HEIGHT = height;

    glViewport(
        0,
        0,
        width,
        height);
}

// ============================================================
// MOUSE
// ============================================================

void mouse_callback(
    GLFWwindow *window,
    double xpos,
    double ypos)
{
    if (primeiro_mouse)
    {
        lastX = xpos;
        lastY = ypos;

        primeiro_mouse = false;
    }

    float xoffset =
        static_cast<float>(
            xpos - lastX);

    float yoffset =
        static_cast<float>(
            lastY - ypos);

    lastX = xpos;
    lastY = ypos;

    const float sensibilidade =
        0.10f;

    xoffset *= sensibilidade;
    yoffset *= sensibilidade;

    Cam_yaw += xoffset;
    Cam_pitch += yoffset;

    if (Cam_pitch > 89.0f)
    {
        Cam_pitch = 89.0f;
    }

    if (Cam_pitch < -89.0f)
    {
        Cam_pitch = -89.0f;
    }

    glm::vec3 frente;

    frente.x =
        cos(
            glm::radians(Cam_yaw)) *
        cos(
            glm::radians(Cam_pitch));

    frente.y =
        sin(
            glm::radians(Cam_pitch));

    frente.z =
        sin(
            glm::radians(Cam_yaw)) *
        cos(
            glm::radians(Cam_pitch));

    Cam_front =
        glm::normalize(frente);
}

// ============================================================
// TECLADO / MOVIMENTO
// ============================================================

void processInput()
{
    float velocidade =
        Cam_speed *
        Tempo_entre_frames;

    glm::vec3 frenteHorizontal =
        glm::normalize(
            glm::vec3(
                Cam_front.x,
                0.0f,
                Cam_front.z));

    glm::vec3 direita =
        glm::normalize(
            glm::cross(
                frenteHorizontal,
                Cam_up));

    if (
        glfwGetKey(
            Window,
            GLFW_KEY_W) == GLFW_PRESS)
    {
        Cam_pos +=
            frenteHorizontal *
            velocidade;
    }

    if (
        glfwGetKey(
            Window,
            GLFW_KEY_S) == GLFW_PRESS)
    {
        Cam_pos -=
            frenteHorizontal *
            velocidade;
    }

    if (
        glfwGetKey(
            Window,
            GLFW_KEY_A) == GLFW_PRESS)
    {
        Cam_pos -=
            direita *
            velocidade;
    }

    if (
        glfwGetKey(
            Window,
            GLFW_KEY_D) == GLFW_PRESS)
    {
        Cam_pos +=
            direita *
            velocidade;
    }

    if (
        glfwGetKey(
            Window,
            GLFW_KEY_Q) == GLFW_PRESS)
    {
        Cam_pos.y +=
            velocidade;
    }

    if (
        glfwGetKey(
            Window,
            GLFW_KEY_E) == GLFW_PRESS)
    {
        Cam_pos.y -=
            velocidade;
    }

    // Liga/desliga o minimapa com M
    if (glfwGetKey(Window, GLFW_KEY_M) == GLFW_PRESS)
    {
        if (!teclaMPressionada)
        {
            mostrarMinimapa = !mostrarMinimapa;
            teclaMPressionada = true;
        }
    }
    else
    {
        teclaMPressionada = false;
    }

    if (
        glfwGetKey(
            Window,
            GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(
            Window,
            true);
    }
}

// ============================================================
// MAIN
// ============================================================

int main()
{
    if (!glfwInit())
    {
        std::cerr
            << "Erro inicializando GLFW."
            << std::endl;

        return -1;
    }

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        4);

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        0);

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE);

    Window =
        glfwCreateWindow(
            WIDTH,
            HEIGHT,
            "Vila Virtual",
            nullptr,
            nullptr);

    if (!Window)
    {
        std::cerr
            << "Erro criando janela."
            << std::endl;

        glfwTerminate();

        return -1;
    }

    glfwMakeContextCurrent(
        Window);

    glfwSetFramebufferSizeCallback(
        Window,
        framebuffer_size_callback);

    glfwSetCursorPosCallback(
        Window,
        mouse_callback);

    glfwSetInputMode(
        Window,
        GLFW_CURSOR,
        GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader(
            (GLADloadproc)glfwGetProcAddress))
    {
        std::cerr
            << "Erro inicializando GLAD."
            << std::endl;

        glfwTerminate();

        return -1;
    }

    glEnable(
        GL_DEPTH_TEST);

    glClearColor(
        0.52f,
        0.72f,
        0.88f,
        1.0f);

    inicializaShaders();

    inicializaTerreno();

    carregaTexturaGrama();

    inicializaCubo();

    inicializaTelhado();

    inicializaSeta();

    // --------------------------------------------------------
    // LOOP PRINCIPAL
    // --------------------------------------------------------

    float ultimoTempo =
        static_cast<float>(
            glfwGetTime());

    while (
        !glfwWindowShouldClose(Window))
    {
        float tempoAtual =
            static_cast<float>(
                glfwGetTime());

        Tempo_entre_frames =
            tempoAtual -
            ultimoTempo;

        ultimoTempo =
            tempoAtual;

        processInput();

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT);

        glUseProgram(
            Shader_programm);

        // ----------------------------------------------------
        // VIEW
        // ----------------------------------------------------

        glm::mat4 view =
            glm::lookAt(
                Cam_pos,
                Cam_pos + Cam_front,
                Cam_up);

        // ----------------------------------------------------
        // PROJECTION
        // ----------------------------------------------------

        float aspecto =
            static_cast<float>(WIDTH) /
            static_cast<float>(HEIGHT);

        glm::mat4 proj =
            glm::perspective(
                glm::radians(67.0f),
                aspecto,
                0.1f,
                200.0f);

        GLint ViewLoc =
            glGetUniformLocation(
                Shader_programm,
                "view");

        GLint ProjLoc =
            glGetUniformLocation(
                Shader_programm,
                "proj");

        glUniformMatrix4fv(
            ViewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(view));

        glUniformMatrix4fv(
            ProjLoc,
            1,
            GL_FALSE,
            glm::value_ptr(proj));

        // ----------------------------------------------------
        // DESENHA VILA
        // ----------------------------------------------------

        desenhaVila();

        if (mostrarMinimapa)
        {
            desenhaMinimapa();
        }

        glfwSwapBuffers(
            Window);

        glfwPollEvents();
    }

    // --------------------------------------------------------
    // LIMPEZA
    // --------------------------------------------------------

    // Cubo
    glDeleteVertexArrays(
        1,
        &VaoCubo);

    glDeleteBuffers(
        1,
        &VboCubo);

    // Telhado
    glDeleteVertexArrays(
        1,
        &VaoTelhado);

    glDeleteBuffers(
        1,
        &VboTelhado);

    // Terreno
    glDeleteVertexArrays(
        1,
        &VaoTerreno);

    glDeleteBuffers(
        1,
        &VboTerreno);

    glDeleteBuffers(
        1,
        &EboTerreno);

    glDeleteTextures(
        1,
        &texturaGrama);

    // Shader
    glDeleteProgram(
        Shader_programm);

    glfwDestroyWindow(
        Window);

    glfwTerminate();

    return 0;
}
