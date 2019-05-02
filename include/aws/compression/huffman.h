#ifndef AWS_COMPRESSION_HUFFMAN_H
#define AWS_COMPRESSION_HUFFMAN_H

/*
 * Copyright 2010-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <aws/compression/exports.h>

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>

#include <stddef.h>

/**
 * Represents an encoded code
 */
struct aws_huffman_code {
    /**
     * The value of the code
     * \note The pattern is stored in the least significant bits
     */
    uint32_t pattern;
    /** The number of bits in pattern to use */
    uint8_t num_bits;
};

/**
 * Function used to encode a single symbol to an aws_huffman_code
 *
 * \param[in] symbol    The symbol to encode
 * \param[in] userdata  Optional userdata (aws_huffman_symbol_coder.userdata)
 *
 * \returns The code representing the symbol. If this symbol is not recognized,
 * return a code with num_bits set to 0.
 */
typedef struct aws_huffman_code(aws_huffman_symbol_encoder_fn)(uint8_t symbol, void *userdata);
/**
 * Function used to decode a code into a symbol
 *
 * \param[in]   bits        The bits to attept to decode a symbol from
 * \param[out]  symbol      The symbol found. Do not write to if no valid symbol
 * found \param[in]   userdata    Optional userdata
 * (aws_huffman_symbol_coder.userdata)
 *
 * \returns The number of bits read from bits
 */
typedef uint8_t(aws_huffman_symbol_decoder_fn)(uint32_t bits, uint8_t *symbol, void *userdata);

/**
 * Structure used to define how symbols are encoded and decoded
 */
struct aws_huffman_symbol_coder {
    aws_huffman_symbol_encoder_fn *encode;
    aws_huffman_symbol_decoder_fn *decode;
    void *userdata;
};

/**
 * Structure used for persistent encoding.
 * Allows for reading from or writing to incomplete buffers.
 */
struct aws_huffman_encoder {
    /* Params */
    struct aws_huffman_symbol_coder *coder;
    uint8_t eos_padding;

    /* State */
    struct aws_huffman_code overflow_bits;
};

/**
 * Structure used for persistent decoding.
 * Allows for reading from or writing to incomplete buffers.
 */
struct aws_huffman_decoder {
    /* Param */
    struct aws_huffman_symbol_coder *coder;

    /* State */
    uint64_t working_bits;
    uint8_t num_bits;
};

AWS_EXTERN_C_BEGIN

/**
 * Initialize a encoder object with a symbol coder.
 */
AWS_COMPRESSION_API
void aws_huffman_encoder_init(struct aws_huffman_encoder *encoder, struct aws_huffman_symbol_coder *coder);

/**
 * Resets a decoder for use with a new binary stream
 */
AWS_COMPRESSION_API
void aws_huffman_encoder_reset(struct aws_huffman_encoder *encoder);

/**
 * Initialize a decoder object with a symbol coder.
 */
AWS_COMPRESSION_API
void aws_huffman_decoder_init(struct aws_huffman_decoder *decoder, struct aws_huffman_symbol_coder *coder);

/**
 * Resets a decoder for use with a new binary stream
 */
AWS_COMPRESSION_API
void aws_huffman_decoder_reset(struct aws_huffman_decoder *decoder);

/**
 * Encode a symbol buffer into the output buffer.
 *
 * \param[in]       encoder         The encoder object to use
 * \param[in]       to_encode       The symbol buffer to encode
 * \param[in]       output          The buffer to write encoded bytes to
 *
 * \return AWS_OP_SUCCESS if encoding is successful, AWS_OP_ERR otherwise
 */
AWS_COMPRESSION_API
int aws_huffman_encode(
    struct aws_huffman_encoder *encoder,
    struct aws_byte_cursor *to_encode,
    struct aws_byte_buf *output);

/**
 * Decodes a byte buffer into the provided symbol array.
 *
 * \param[in]       decoder         The decoder object to use
 * \param[in]       to_decode       The encoded byte buffer to read from
 * \param[in]       output          The buffer to write decoded symbols to
 *
 * \return AWS_OP_SUCCESS if encoding is successful, AWS_OP_ERR otherwise
 */
AWS_COMPRESSION_API
int aws_huffman_decode(
    struct aws_huffman_decoder *decoder,
    struct aws_byte_cursor *to_decode,
    struct aws_byte_buf *output);

AWS_EXTERN_C_END

#endif /* AWS_COMPRESSION_HUFFMAN_H */
