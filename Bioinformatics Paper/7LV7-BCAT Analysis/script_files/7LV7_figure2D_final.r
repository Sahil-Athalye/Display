# Set plotting parameters for a white background
par(bg = "white", mar = c(6, 4, 4, 2) + 0.1)

# Define the data vectors for Region 1
code1 <- c("V223", "E224", "E225", "F226", "S227", "T228", "S229", "N230", "F231", "F232")
number1 <- c(2, 4, 8, 0, 2, 1, 0, 8, 3, 8)

# Define the data vectors for Region 2
code2 <- c("G236", "A237", "T238", "G239", "T300", "A301", "V302", "V303", "V304", "T305", "P306", "I307")
number2 <- c(1, 3, 7, 2, 5, 3, 6, 7, 2, 4, 6, 7)

# Combining the data for plotting
combined_numbers <- c(number1, number2)
combined_codes <- c(code1, code2)

# Colors for the bars
bar_colors <- c(rep("red", length(number1)), rep("blue", length(number2)))

# Create the merged bar chart
barplot_heights <- barplot(combined_numbers, 
                           names.arg = NULL, 
                           col = bar_colors, 
                           main = "Conserved Residues in Region 1 and 2", 
                           xlab = "", 
                           ylab = "Number of Structural Homologs with Identical Residues", 
                           ylim = c(0, max(combined_numbers) + 1))

# Add labels directly below each bar
text(cex=0.6, x=barplot_heights, y=-0.5, labels=combined_codes, srt=0, xpd=TRUE)

# Add the x-axis label below the custom labels
mtext("Amino Acid Residues", side=1, line=2)

# Add grid for better readability
grid(nx = NA, ny = NULL)

# Add the legend at the top outside the plot area
legend("topright", inset=c(-0.2,-0.1), legend=c("Region 1", "Region 2"), fill=c("red", "blue"), cex=0.8, bty="n", x.intersp = 0.5, y.intersp = 0.5)