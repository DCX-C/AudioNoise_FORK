struct {
	struct lfo_state lfo;
	struct biquad_coeff coeff;
	float s0[2], s1[2], s2[2], s3[2];
	float center_f, octaves, Q, feedback;
} phaser;

void phaser_init(float pot1, float pot2, float pot3, float pot4)
{
	float ms = cubic(pot1, 25, 2000);		// 25ms .. 2s
	set_lfo_ms(&phaser.lfo, ms);
	phaser.feedback = linear(pot2, 0, 0.75);

	phaser.center_f = pot_frequency(pot3);		// 220Hz .. 6.5kHz
	phaser.octaves = 0.5;				// 155Hz .. 9kHz
	phaser.Q = linear(pot4, 0.25, 2);

	float lo = pow2(-phaser.octaves) * phaser.center_f;
	float hi = pow2( phaser.octaves) * phaser.center_f;

	fprintf(stderr, "phaser:");
	fprintf(stderr, " lfo=%g ms", ms);
	fprintf(stderr, " f=%.0f (%.0f - %.0f) Hz", phaser.center_f, lo, hi);
	fprintf(stderr, " feedback=%g", phaser.feedback);
	fprintf(stderr, " Q=%g\n", phaser.Q);
}

float phaser_step(float in)
{
	float lfo = lfo_step(&phaser.lfo, lfo_triangle);
	float freq = pow2(lfo*phaser.octaves) * phaser.center_f;
	float out;

	_biquad_allpass_filter(&phaser.coeff, freq, phaser.Q);

	out = in + phaser.feedback * phaser.s3[0];
	out = biquad_step_df1(&phaser.coeff, out, phaser.s0, phaser.s1);
	out = biquad_step_df1(&phaser.coeff, out, phaser.s1, phaser.s2);
	out = biquad_step_df1(&phaser.coeff, out, phaser.s2, phaser.s3);

	return limit_value(in + out);
}
