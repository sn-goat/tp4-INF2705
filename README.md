# TP4 - Sword 'n Magic

## Description générale

Scène 3D interactive sur le thème "épée et magie". Deux personnages
bâtonnets se font face, l'un tient une épée, tandis que l'autre a un bâton magique.
Une spline de Bézier cubique animée relie les deux armes, et le fond
est un gradient animé. Les armes sont des modèles 3D texturés éclairés
par un modèle d'illumination locale interactif.

## Concepts utilisés

### 1. Modèle d'illumination locale Blinn-Phong (cours 6)

Implémenté sur les deux armes avec deux sources (une lumière
directionnelle sans atténuation et une lumière ponctuelle avec
atténuation `1/d^2`) et les trois composantes :
- **Ambiante** : `ka * Clumière`.
- **Diffuse (loi de Lambert)** : `kd * Clumière * (N^T L)`.
- **Spéculaire Blinn-Phong** : `ks * Clumière * (N^T B)^σ`,
  avec `B = (L + V) / 2`.  


**Amélioration du rendu** : 
- Un dégradé diffus qui donne du volume à l'épée et au bâton selon
  l'orientation de leurs faces par rapport à la lumière.
- Un reflet spéculaire brillant sur la lame de l'épée (brillance
  élevée, aspect métallique) et un reflet plus doux sur le bâton
  (brillance faible, aspect bois).

Les coefficients de matériau (`ka`, `kd`, `ks`, brillance) sont
différents pour chaque arme afin de rendre leurs surfaces distinctes.

### 2. Calcul d'illumination par fragment - Phong shading (cours 6)

L'illumination est calculée dans le fragment shader, avec les normales interpolées, et une
matrice de normales correcte pour l'épée et le bâton magique:
```
swordNormalMatrix = transpose(inverse(mat3(swordModelView)));
staffNormalMatrix = transpose(inverse(mat3(staffModelView)));
```

**Amélioration du rendu** :
- Les reflets spéculaires sont lisses au lieu d'être
  interpolés grossièrement entre sommets (ce qui donnerait des
  défaults visuels).
- Les normales restent correctes même quand les armes subissent
  des rotations ou changements d'échelle dans leurs transformations
  (grâce à la matrice des normales).

Les lumières sont transformées chaque trame dans le repère de la
caméra (view * vec4(pos, 1) pour les positions, view * vec4(dir, 0)
pour les directions) afin de rester cohérent avec le fait que les
calculs se font en espace de vue (caméra à l'origine).
