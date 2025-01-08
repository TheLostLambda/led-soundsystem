using Plots
using FFTW

# Sample period
Ts = 1 / (44140)

function plotfft(signal)
    # Number of points 
    N = length(signal)
    # Time coordinate
    t = range(0, step=Ts, length=N)

    # Fourier Transform of it 
    F = fft(signal) |> fftshift
    freqs = fftfreq(length(t), 1.0/Ts) |> fftshift

    # Plotting
    ms = 1000t
    time_domain = plot(ms, signal, title = "Signal")
    freq_domain = plot(freqs, abs.(F), title = "Spectrum", xlim=(0, +250)) 
    plot(time_domain, freq_domain, layout = 2)
end

function stereoplot(left, right)
    left = plotfft(left .- 126)
    right = plotfft(right .- 126)
    plot(left, right, layout = (2, 1))
end
