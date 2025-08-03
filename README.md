# GPT Implementation in C

A from-scratch implementation of a GPT (Generative Pre-trained Transformer) model in pure C, focusing on educational purposes and deep understanding of transformer architectures.

## Project Status

**Current Implementation: ~35% Complete**

This project implements the foundational components of a GPT model with a focus on clarity and correctness. The core attention mechanism and tokenization systems are complete, but several key components remain to be implemented.

## Implemented Components

### Core Mathematical Operations
- [x] **Matrix Library** (`src/util/matrix.c`)
  - Efficient matrix operations with row-major storage
  - Matrix multiplication, addition, transpose
  - Memory-optimized flat array representation
  - Proper initialization and cleanup

- [x] **Dynamic Arrays** (`src/util/array.c`)
  - Generic resizable arrays with type safety
  - Dot product operations for vectors
  - Array utilities (join, duplicate, compare)

- [x] **Tensor Operations** (`src/util/tensor.c`)
  - Matrix splitting for multi-head attention
  - Tensor concatenation for combining attention heads

### Multi-Head Self-Attention
- [x] **Complete Attention Mechanism** (`src/attention/attention.c`)
  - Scaled dot-product attention (Q, K, V projections)
  - Multi-head attention with proper head splitting
  - Causal masking for autoregressive generation
  - Numerically stable softmax implementation
  - Dropout with proper scaling (1/(1-p))
  - Output projection matrix

### Tokenization System
- [x] **Byte-Pair Encoding (BPE)** (`src/token/`)
  - Full BPE algorithm for vocabulary construction
  - Custom hashmap with FNV-1a hashing
  - Support for RAW, EOW (end-of-word), EOS tokens
  - Vocabulary serialization/deserialization
  - Text encoding and decoding with word boundaries
  - Token embedding into matrices

### Data Handling
- [x] **Data Loader** (`src/loader/data_loader.c`)
  - Sliding window batch generation
  - Configurable sequence length and batch size

### Configuration
- [x] **Model Parameters** (`include/config/config.h`)
  - GPT-2 style configuration (50,257 vocab, 768 dim, 12 heads/layers)
  - Context length: 1,024 tokens
  - 10% dropout rate

## Usage

### Build Vocabulary
```bash
make train_vocab file=input.txt vocab_size=8000
```

### Tokenize Text
```bash
make tokenize file=input.txt
```

### Clean Build
```bash
make clean
```

## Missing Components (To Be Implemented)

### Critical Components
- [ ] **Layer Normalization** - Essential for training stability
- [ ] **Feed-Forward Network (MLP)** - Position-wise neural networks with GELU activation
- [ ] **Positional Encoding** - Learned or sinusoidal position embeddings
- [ ] **Transformer Block** - Integration of attention + FFN + layer norm + residuals
- [ ] **Complete GPT Model** - Full model architecture with multiple layers
- [ ] **Training Loop** - Forward/backward pass with loss computation and optimization
- [ ] **Text Generation** - Inference engine with sampling strategies

### Supporting Components
- [ ] **Activation Functions** - GELU, ReLU implementations with derivatives
- [ ] **Weight Initialization** - Xavier/He initialization schemes
- [ ] **Loss Functions** - Cross-entropy loss for language modeling
- [ ] **Optimizers** - Adam/SGD with gradient computation
- [ ] **Model Persistence** - Weight saving/loading and checkpointing

### Advanced Features
- [ ] **Regularization** - L1/L2 regularization and gradient clipping
- [ ] **Performance Optimizations** - BLAS integration, SIMD vectorization
- [ ] **Advanced Sampling** - Top-k, nucleus (top-p), temperature sampling
- [ ] **Flash Attention** - Memory-efficient attention computation

## Architecture

```
src/
├── attention/          # Multi-head self-attention mechanism
├── loader/            # Data loading and batching
├── token/             # BPE tokenization and vocabulary
└── util/              # Core utilities (matrix, array, tensor operations)

include/               # Header files mirroring src/ structure
run/                   # Executable programs for vocab training and tokenization
```

## Design Principles

- **Educational Focus**: Clear, readable code prioritizing understanding over performance
- **Memory Safety**: Consistent malloc/free patterns with proper cleanup
- **Numerical Stability**: IEEE 754 compliant implementations with stable algorithms
- **Modular Design**: Clean separation between components for easy testing and extension

## Key Implementation Details

### Attention Mechanism
- Implements scaled dot-product attention: `Attention(Q,K,V) = softmax(QK^T/√d_k)V`
- Causal masking prevents future token leakage
- Dropout applied after softmax for regularization
- Multiple heads processed independently then concatenated

### Tokenization
- BPE algorithm learns vocabulary from training data
- Supports byte-level encoding for handling any Unicode text
- Custom hash table implementation for efficient token lookup
- Preserves word boundaries with special end-of-word tokens

### Matrix Operations
- Row-major storage for cache efficiency
- In-place operations where possible to minimize memory allocation
- Numerical stability prioritized (e.g., stable softmax with max subtraction)

## Next Steps

1. **Layer Normalization** - Start with this as it's used throughout the transformer
2. **Feed-Forward Network** - Implement the MLP layers with GELU activation
3. **Transformer Block** - Combine attention + FFN + normalization + residuals
4. **Position Embeddings** - Add positional information to token embeddings
5. **Complete Model** - Stack transformer blocks into full GPT architecture

## Requirements

- GCC compiler
- Standard C library
- POSIX-compliant system for file operations

## Learning Resources

This implementation follows the transformer architecture from:
- "Attention Is All You Need" (Vaswani et al., 2017)
- GPT-2 architecture specifications
- Modern transformer best practices
